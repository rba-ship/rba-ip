

#include "../FreeRTOS/FreeRTOS.h"
#include "../FreeRTOS/queue.h"
#include "../app_queue/app_queue.h"
#include "../app_Task/app_Task.h"
#include "DMA.h"
#include "ADS1299.h"
#include "buzzerself.h"
#include "Data_Pack.h"



//#define BUF_SIZE 512
//uint8_t g_ads_rx_buf[ADS1299_DATA_LEN];
//uint8_t g_ads_tx_dummy[ADS1299_DATA_LEN] = {0}; // 用于产生SPI时钟的空数据
//volatile uint8_t g_data_ready_flag = 0;
//uint8_t data1=0;

//volatile uint8_t ADC_BufA[512] = {0}; 
//volatile uint8_t ADC_BufB[512] = {0};
volatile uint8_t Buffer_Full_Flag = 0;


// 32 段 x 24 字节 = 768 字节原始采样数据
//#define ADS1299_RAW_PACKET_SIZE  24
//#define ADS1299_DMA_BATCH_COUNT  32
//#define DMA_RX_BUF_SIZE         (ADS1299_RAW_PACKET_SIZE * ADS1299_DMA_BATCH_COUNT)

//uint8_t g_dma_rx_buf[DMA_RX_BUF_SIZE] __attribute__((aligned(4)));


void SPI1_DMA_Init(void) {
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 1. 开启 DMA2 时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    // 2. 复位 Stream
    DMA_DeInit(DMA2_Stream0);

    // 3. 基本配置
    DMA_InitStructure.DMA_Channel = DMA_Channel_3;              // SPI1_RX 通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR; // 外设地址：SPI数据寄存器
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)g_dma_buf_a; // 内存地址0，如有需要请更改
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;     // 方向：外设到内存
    DMA_InitStructure.DMA_BufferSize = RAW_BUF_SIZE;                // 缓冲区大小,如有需要请更改
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不增加
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;     // 内存地址增加
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 16位数据
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 16位数据
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;             // 循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;         // 高优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    DMA_Init(DMA2_Stream0, &DMA_InitStructure);

    // 4. 关键：开启双缓冲模式
    // 参数：Stream, 第二个缓冲区地址, 当前首选的缓冲区，如有需要请更改。
    DMA_DoubleBufferModeConfig(DMA2_Stream0, (uint32_t)g_dma_buf_b, DMA_Memory_0);
    DMA_DoubleBufferModeCmd(DMA2_Stream0, ENABLE);

    // 5. 开启传输完成中断
    DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);

    // 6. NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x05;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 7. 启动 DMA (等待 SPI 启动信号) 
		DMA_Cmd(DMA2_Stream0, ENABLE);
		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
		SPI_Cmd(SPI1, ENABLE);
   
}

//数据从内存区域搬运至串口，并由串口传输数据，数据所处的内存区域请参见app——Task.h文件
//本函数是作为DMA配置的初始化函数
void USART_DMA_Init(void) {

    DMA2_Stream7->CR &= ~DMA_SxCR_EN;
    while (DMA2_Stream7->CR & DMA_SxCR_EN);

    DMA2_Stream7->PAR = USART1_BASE + 0x04;//传输到外设的数据寄存器地址
    DMA2_Stream7->M0AR = (uint32_t)&tx_pack;
	  //DMA2_Stream7->M1AR = (uint32_t)&tx_pack[1];
    DMA2_Stream7->NDTR = sizeof(Pack1024_t);
    DMA2_Stream7->CR |= DMA_SxCR_CHSEL_4;//选择通道4
    DMA2_Stream7->CR |= DMA_SxCR_PL_1;
    DMA2_Stream7->CR |= DMA_SxCR_MSIZE_1;
    DMA2_Stream7->CR |= DMA_SxCR_PSIZE_1;

    DMA2_Stream7->CR &= ~DMA_SxCR_MSIZE;           // MSIZE[1:0] = 00  (内存数据宽度: 8位 Byte)
    DMA2_Stream7->CR &= ~DMA_SxCR_PSIZE;           // PSIZE[1:0] = 00  (外设数据宽度: 8位 Byte)
    DMA2_Stream7->CR |= DMA_SxCR_MINC;             // MINC       = 1   (内存地址递增)
    DMA2_Stream7->CR &= ~DMA_SxCR_PINC;            // PINC       = 0   (外设地址固定)
	  //DMA2_Stream7->CR |= DMA_SxCR_DBM;               // 使能双缓冲模式 (硬件自动强制开启 Circular 循环)
    DMA2_Stream7->CR &= ~DMA_SxCR_CT;               // 设置当前（首个）传输的目标内存为 Memory 0 (CT=0)
	  DMA2_Stream7->CR &= ~DMA_SxCR_CIRC;
    DMA2_Stream7->CR |= DMA_SxCR_DIR_0;//(1 << DMA_SxCR_DIR_Pos);   // DIR[1:0]   = 01  (传输方向: 内存 -> 外设)
    DMA2_Stream7->CR |= DMA_SxCR_TCIE;             // TCIE       = 1   (使能传输完成中断)
	
	//donot enable
	  //DMA2_Stream7->CR |= DMA_SxCR_EN;


    NVIC_SetPriority(DMA2_Stream7_IRQn, 6);        // 优先级必须低于 FreeRTOS 管理阈值
    NVIC_EnableIRQ(DMA2_Stream7_IRQn);

    USART1->CR3 |= USART_CR3_DMAT;


}




void ADS1299_EXTI_Init(void ){
	  EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
	  
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); // SYSCFG 必须开 
	
	  NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x05;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStruct);
	
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource3);
	
	  EXTI_InitStruct.EXTI_Line = EXTI_Line3;   //PC1对应EXTIline1
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt; // 中断模式
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

}





// --- 中断服务函数 ---

// 1. DRDY 触发的中断
void EXTI3_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line3) != RESET) {
            //进入了中断，进行数据收集
			ADS1299_Get_DATA_DMA();

            //对数据进行打包
           
			  

			
			EXTI_ClearITPendingBit(EXTI_Line3);
    }
}





//DMA传输至768字节的区域的中断函数，DMA中断是自己进行搬运的，无需其他程序进行搬运。
void DMA2_Stream0_IRQHandler(void) {
    // 检查是否是传输完成中断
    if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0)) {
        
        // 检查当前 DMA 正在指向哪一个内存区域
        // 如果当前正在往 Memory1 (BufB) 写，说明 Memory0 (BufA) 已经填满了
        if (DMA_GetCurrentMemoryTarget(DMA2_Stream0) == 1) {
            Buffer_Full_Flag = 1; 
					  SPI_DMA_M0_CpltCallback();
					//printf("0");
        } else {
					  SPI_DMA_M1_CpltCallback();
            Buffer_Full_Flag = 2;
        }

        // 清除标志位
        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
    }
}




/* ===========================================================================
 * 3.库回调函数：Memory 0 (buf0) 填满 768 字节时自动触发
 * =========================================================================== */
void SPI_DMA_M0_CpltCallback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	  uint8_t *p_buf = g_dma_buf_a;
    //uint8_t *p_buf = g_dma_buf_a; // 拿到已满的 buf0 地址

    if (xDmaPointerQueue != NULL) {
        // 1. 将 buf0 的地址塞入 FreeRTOS 队列 (零拷贝，只传 4 字节指针)
        xQueueSendFromISR(xDmaPointerQueue, &p_buf, &xHigherPriorityTaskWoken);

        // 2. 触发上下文切换：如果打包 Task 优先级高，退出中断后内核会立刻跳转去打包
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}


/* ===========================================================================
 * 4. HAL 库回调函数：Memory 1 (buf1) 填满 768 字节时自动触发
 * =========================================================================== */
void SPI_DMA_M1_CpltCallback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	  uint8_t *p_buf = g_dma_buf_b;
    //uint8_t *p_buf = g_dma_buf_b; // 拿到已满的 buf1 地址

    if (xDmaPointerQueue != NULL) {
        // 1. 将 buf1 的地址塞入 FreeRTOS 队列
        xQueueSendFromISR(xDmaPointerQueue, &p_buf, &xHigherPriorityTaskWoken);
        
        // 2. 触发上下文切换
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}


void DMA2_Stream7_IRQHandler(void) {
    // 读取高位中断状态寄存器 HIFR 的 TCIF7 (Bit 27)
    if (DMA2->HISR & DMA_HISR_TCIF7) {
																											
        // 向高位中断清除寄存器 HICR 的 CTCIF7 (Bit 27) 写 1 清零标志位
        DMA2->HIFCR = DMA_HIFCR_CTCIF7;

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        // 唤醒串口发送任务
        vTaskNotifyGiveFromISR(uartTask, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}


