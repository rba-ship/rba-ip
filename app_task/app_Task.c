#include "app_Task.h"
#include "Data_Pack.h"
#include "DMA.h"
//#include "FreeRTOS.h"
//#include "queue.h"
#include "lwip/sockets.h"
#include "app_lwip/ethernetif.h"
#include "LAN8742A.h"
#include "app_lwip/app_lwip.h"
#include "app_semphr.h"
//#include "../FreeRTOS/task.h"


StaticTask_t xTaskBuffer;
StaticTask_t xTaskBuffer1;
StaticTask_t xTaskBuffer2;
StaticTask_t xTaskBuffer3;
StaticTask_t xTaskBuffer4;


TaskHandle_t Task1 = NULL;
TaskHandle_t uartTask = NULL;
TaskHandle_t TcpTask = NULL;
TaskHandle_t EthInTask = NULL;
TaskHandle_t xAppTask = NULL;

/******************************************************/
/*              内存空间开辟                          */
/*****************************************************/
StackType_t xStack[ STACK_SIZE_1024 ];
StackType_t xStackuart[ STACK_SIZE_512];
StackType_t xStackTcp[ STACK_SIZE_1024 ];
StackType_t xStackEthIn[ STACK_SIZE_4096 ];
StackType_t xStackApp[ STACK_SIZE_4096 ];











/******************************************************/
/*Task Create Block                                   */
/*请在这里创建任务    */   
/*
void vTask(void* pvParameters) {
			ADCFrameCreate(rx_frame);

}
*/
volatile uint32_t semTakeCount = 0;


//打包函数
/*
void vUpperComputerTxTask(void *pvParameters) {
    //ADS1299_Frame_t rx_frame;
	
	  //创建把数据结构,rx_frmae代表了一个32字节的数据结构。
		ADCFrameCreate(&rx_frame);
	
	  //创建一个queue，作为数据处理的队列数据结构
	  Q1=dataQueueCreate();
    
    // 开辟一个本地发送缓冲区，最多一次打包 32 帧 (32 * 32 字节 = 1024 字节),《不要在这里开辟缓冲区》
    uint8_t tx_buffer[1024];
    uint16_t frame_count = 0;

    for (;;) {
        // 【步骤 1】死等队列第一帧数据
        // portMAX_DELAY 表示无数据时彻底挂起任务，不占用任何 CPU
        if (xQueueReceive(g_xADS1299Queue, &rx_frame, portMAX_DELAY) == pdTRUE) {
            
            // 拷贝第一帧到发送缓冲区
            memcpy(&tx_buffer[0], &rx_frame, sizeof(ADC32_Frame_t));
            frame_count = 1;

            // 【步骤 2】尝试把队列里积压的后续数据“一口气掏空”
            // timeout 设为 0，有就拿，没有立刻退出循环，绝不等待
            while (frame_count < 32 && 
                   xQueueReceive(g_xADS1299Queue, &rx_frame, 0) == pdTRUE) {
                
                memcpy(&tx_buffer[frame_count * sizeof(ADC32_Frame_t)], 
                       &rx_frame, 
                       sizeof(ADC32_Frame_t));
                frame_count++;
            }

            // 【步骤 3】将打包好的数据通过物理接口发送给上位机
            uint16_t total_bytes = frame_count * sizeof(ADC32_Frame_t);
            
            // 方式 A：使用 USB CDC 虚拟串口发送（推荐，速度极快）
            // CDC_Transmit_FS(tx_buffer, total_bytes);
            
            // 方式 B：使用串口 DMA 发送（注意：需确保上次 DMA 发送已结束）
            // HAL_UART_Transmit_DMA(&huart1, tx_buffer, total_bytes);
            
            // 方式 C：普通串口阻塞发送（仅限波特率极高、数据量小的情况）
           // HAL_UART_Transmit(&huart1, tx_buffer, total_bytes, 100);
        }
    }
}
*/

//数据打包任务
void vDataRouterTask(void *pvParameters){
	
	 
	uint8_t i=0;
	uint8_t index =0;
	//创建指针
	//uint8_t *p_raw_864bytes = NULL;
	//创建结构体
	static ADC32_Frame_t ADS1299_Frame_t;
	//static Pack1024_t tx_pack;
	uint8_t *p_buf_a = g_dma_buf_a;
	uint8_t *p_buf_b = g_dma_buf_b;



	//创建一个打包的数据结构
	//ADCFrameCreate( & ADS1299_Frame_t);
	// (b) 计算 864 字节原始数据中第 i 块 27B raw 数据的偏移地址
  //uint8_t *p_single_raw_27_a = &p_buf_a[i * 27];
	//uint8_t *p_single_raw_27_b = &p_buf_b[i * 27];
	configASSERT( ( uint32_t ) pvParameters == 1UL );
	for(;;){
		//if(Buffer_Full_Flag == 1){
			if (xQueueReceive(xDmaPointerQueue, &p_buf_a, portMAX_DELAY) == pdTRUE) {
			
			for(i=0;i<32;i++){
				  uint8_t *p_single_raw_27_a = &p_buf_a[i * 27];
					ADCFrameCreate( &tx_pack.frames[i]);

                // (c) 将这 27 字节 raw 数据精准拷贝到第 i 帧的 raw_data 字段中
          memcpy(tx_pack.frames[i].ch_data, p_single_raw_27_a, 27);
				
			}
			index ^= 1;
			
			
			xQueueSend(xUartTxQueue, &tx_pack, portMAX_DELAY);
			xTaskNotifyGive(TcpTask);
			
			//xQueueSend(xUartTxQueue, &tx_pack, portMAX_DELAY);
			//xTaskNotifyGive(uartTask);
		}
	}
	
	
	/*
	
   // else if(Buffer_Full_Flag == 2){
			else if(xQueueReceive(xDmaPointerQueue, &p_buf_b, portMAX_DELAY) == pdTRUE) {
			
						for(i=0;i<32;i++){
							  uint8_t *p_single_raw_27_b = &p_buf_b[i * 27];
								ADCFrameCreate( &tx_pack.frames[i]);
								
				

                // (c) 将这 27 字节 raw 数据精准拷贝到第 i 帧的 raw_data 字段中
								memcpy(tx_pack.frames[i].ch_data, p_single_raw_27_b, 27);
		
						}
			xQueueSend(xUartTxQueue, &tx_pack, portMAX_DELAY);
			xTaskNotifyGive(uartTask);
			}
	*/
	//	}
		//}

	//}
}

//创建任务，任务的优先级是4
void RouterTaskCreate(void){
	
	//TaskHandle_t
	 Task1 = xTaskCreateStatic(vDataRouterTask,
	                                    "PACK",
								STACK_SIZE_1024,
									(void *)1 ,
										 PRI_6,
									    xStack,
								 &xTaskBuffer);





}


//一次采样，传输的数据帧是36个字节，senddata函数传输的字节是16字节。
void vUpperComputerTxSerialTask(void* pvParameters) {
	for (;;) {
		//USART_SendData(USART1,);
		if (xQueueReceive(xUartTxQueue, &tx_pack, portMAX_DELAY) == pdTRUE) {
			//DMA2_Stream7->CR |= DMA_SxCR_EN;
			
	  //DMA2_Stream7->CR |= DMA_SxCR_EN;
			//DMA2_Stream7_IRQHandler();
			
			
			// Defensive: make sure any previous transfer is fully stopped
     DMA2_Stream7->CR |= DMA_SxCR_EN;
     while (DMA2_Stream7->CR & DMA_SxCR_EN);
     DMA2->HIFCR = DMA_HIFCR_CTCIF7;      // clear stale flag before restart

     DMA2_Stream7->M0AR = (uint32_t)&tx_pack;
     DMA2_Stream7->NDTR = sizeof(Pack1024_t);      // see note below — do NOT assume sizeof(Pack1024_t)==FRAME_LEN
     //DMA2_Stream7->CR  |= DMA_SxCR_EN;    // kick off this frame's transfer
			
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);


		}
		
		
		

			
	}




}

//创建任务，任务的优先级是
void vUpperComputerTxSerialTaskCreate(void){
	
	//TaskHandle_t
	uartTask = xTaskCreateStatic(vUpperComputerTxSerialTask,
	                                    "UART",
								STACK_SIZE_512,
									(void *)1 ,
										 PRI_4,
									    xStackuart,
								 &xTaskBuffer1);





}



/**
 * @brief 纯 TCP 网口发送任务（替代串口 DMA 发送任务）
 */
void vUpperComputerTxTcpTask(void *pvParameters)
{
    int server_fd = -1;
    int client_fd = -1;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size = sizeof(struct sockaddr_in);

    /* 1. 创建 TCP Socket */
    //server_fd = socket(AF_INET, SOCK_STREAM, 0);

    /* 2. 绑定本地端口（如 8080） */
    //memset(&server_addr, 0, sizeof(server_addr));
    //server_addr.sin_family = AF_INET;
    //server_addr.sin_port = htons(8080);
    //server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
   
    /* 3. 开始监听 */
    listen(server_fd, 5);
	
    //printf("TCP Server Listening on port 8080...\r\n"); // 确认是否打印此行
    for (;;) 
    {
        /* 4. 阻塞等待上位机 (NetAssist) 连接 */
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &sin_size);

        if (client_fd >= 0) 
					 //printf("NetAssist Connected!\r\n,%d \n",client_fd);
       // {
            for (;;) 
            {
                /* 5. 从队列提取打包好的 tx_pack 数据包 */
                if (xQueueReceive(xUartTxQueue, &tx_pack, portMAX_DELAY) == pdTRUE) 
                {
                    /* 6. 直接通过 TCP Socket 发送完整结构体 */
                    int ret = send(client_fd, &tx_pack, sizeof(Pack1024_t), 0);
										//printf("Send Data...\n  %d  \n",ret);

                    /* 7. 若上位机断开连接，关闭当前 Socket 并重新等待连接 */
                    if (ret <= 0) {
											  //printf("\n NetAssist Disconnected!\r\n");
                        lwip_close(client_fd);
                        client_fd = -1;
                        break;
                    }
                }
            }
        }
    }
//}

/**
 * @brief 静态创建 TCP 发送任务
 */
void TcpTxTaskCreate(void)
{
    //static StackType_t xStack[1024];
    //static StaticTask_t xTaskBuffer;

    TcpTask = xTaskCreateStatic(vUpperComputerTxTcpTask,
                      "TCP_TX",
                      STACK_SIZE_1024,
                      (void *)1,
                      PRI_4,                 /* 任务优先级 */
                      xStackTcp,
                      &xTaskBuffer2);
}


void ethernetif_input_task(void *argument)
{
    struct netif *netif = (struct netif *)argument;

    for (;;)
    {
			if (xSemaphoreTake(EthSemaphore, portMAX_DELAY) == pdTRUE)
        {
            // 被中断唤醒后提取数据包并重置 RX 描述符 OWN 位
            ethernetif_input(netif);
						semTakeCount++;
					  //printf("eth\n");
						//if ((ETH->DMASR & ETH_DMASR_RBUS) != RESET) {
            //    ETH->DMASR = ETH_DMASR_RBUS; // 清除 RBUS 状态
            //    ETH->DMARPDR = 0;            // 重新唤醒 DMA 接收
            //}
        }
        //ethernetif_input(netif);
    }
}

void EthInTaskCreate(void)
{
    //static StackType_t xStack[1024];
    //static StaticTask_t xTaskBuffer;

    EthInTask = xTaskCreateStatic(ethernetif_input_task,
                      "ETH_IN",
                      STACK_SIZE_4096,
                      &gnetif,
                      PRI_4,                 /* 任务优先级 */
                      xStackEthIn,
                      &xTaskBuffer3);
}

void App_Task(void *argument){
		BaseType_t xReturn = pdPASS;
		//for(;;){
		native_lwip_init();
	  //EthInTaskCreate();
	  TCP_Server_Init(8080);
		//send(client_fd,test,strlen(test),0);

	  //printf("app_task..");
		//for(;;){
	//}
		vTaskDelete(NULL); //删除AppTaskCreate任务
		//}

}

void AppTaskCreate(void)
{
    //static StackType_t xStack[1024];
    //static StaticTask_t xTaskBuffer;
	 // BaseType_t xAppTask=pdPASS;

    xAppTask = xTaskCreateStatic(App_Task,
                      "APP_TASK_F",
                      STACK_SIZE_4096,
                      (void *)1,
                      PRI_1,                 /* 任务优先级 */
                      xStackApp,
                      &xTaskBuffer4);
	//if(pdPASS == xAppTask)
   // vTaskStartScheduler();   /* 启动任务，开启调度 */
  //else							
}

/*
void TcpTestTask(void *pvParameters){
	send(client_fd,test,strlen(test),0);


}
*/
