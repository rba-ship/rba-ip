/**
  ******************************************************************************
  * 本main函数由任柏安设计，主要功能是用于驱动ADS1299及进行数据采集，并将原始数据传送到SD卡内部，用于保存数据。
  * 
  * 主要的设计特点：
  * 1.数据搬运均采用DMA进行搬运，减少了CPU的使用量，提高了数据转换的效率。
  * 2.采用4线软件SPI通信协议。具体的通信协议参阅SPI协议。
  * 3.文件系统采用FATFS文件系统。
  * 4.代码采用STM32F4标准库。
  * 
  * 注意事项：
  * SD卡（小卡）数据存储方式：数据存储以512 Byte为一个小的存储单元，即以512byte为一个单元进行存储，SD_num代表了需要存储多少个512 Byte
  * 注意：ADS1299的数据格式（数据为16进制）为 C0 00 00 XX[共3*8=24个转换后的数据]
  * 引脚的连接请参阅设计pdf文件和其他头文件
  * SD卡选择请注意检查文件系统是否为FAT32，推荐使用32GB容量大小的SD卡。
  * 
  * 
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "buzzerSelf.h"
#include "ADS1299.h"
#include "bsp_usart.h"
#include "delay.h"
#include "DMA.h"
#include "sdio_sd.h"
#include "ff.h"
#include "Data_pack.h"
#include <stdio.h>
#include "../app_queue/app_queue.h"
#include "../app_Task/app_task.h"
#include "../app_lwip/app_lwip.h"
#include "../app_semphr/app_semphr.h"
#include "LAN8742A.h"
#include "etharp.h"
#include "stm32f429_eth.h"

//#define DEBUG
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */

//void Delay(__IO u32 nCount);
#ifdef DEBUG
volatile int res=0;  //变量：用于存储Fat FS文件的挂载打开所返回的代码
#endif
//static void ADS_CS_Low(void) { GPIO_ResetBits(GPIOA, GPIO_Pin_4); }
//static void ADS_CS_High(void) { GPIO_SetBits(GPIOA, GPIO_Pin_4); }
FATFS fs; //挂载文件系统
FIL file;
UINT fnum;
int errno = 0;
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */

int main(void)
{ 
	//char task_stats_buf[256];
	//配置写入的SD的block
	int SD_num=100000;
	//Buffer_Full_Flag=0;
	//char csv_buffer[1024]; // 用于格式化字符串
	int num=0;
    //uint8_t chip_id = 0;
	uint8_t data=0;

	//ADCFrameCreate(&p);
  //volatile uint32_t i; // 变量声明必须放在函数开头
		/* 程序来到main函数之前，启动文件：statup_stm32f4xx.s已经调用
		* SystemInit()函数把系统时钟初始化成168MHZ
		* SystemInit()在system_stm32f4xx.c中定义
		* 如果用户想修改系统时钟，可自行编写程序修改
		*/
		  /* add your code here ^_^. */
	Ethernetif_Semaphore_Create();

  //队列创建
	CreatexDmaPointerQueue();
	//Createg_xADS1299Queue();
	CreatexUartTxQueue();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	USARTx_Config();
  ETH_BSP_Config();
	uint8_t rawPrio = NVIC->IP[ETH_IRQn];
  printf("ETH_IRQn raw priority byte = 0x%02X\r\n", rawPrio);
	uint16_t bsr = ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, PHY_BSR);
  printf("PHY BSR = 0x%04X, link %s\r\n", bsr, (bsr & PHY_Linked_Status) ? "UP" : "DOWN");
	EthInTaskCreate();


	
  AppTaskCreate();
  //RouterTaskCreate();
	//vUpperComputerTxSerialTaskCreate();
  //native_lwip_init();
	//TcpTxTaskCreate();
	
  Delay_us_Init();
	gpioBuzzerConfig();
#ifdef DEBUG
	printf("1111");
#endif
	
	ADS1299_SPI_Init();
	//ADS1299_DMA_Init();
	
	ADS1299_Init();
	//vUpperComputerTxSerialTaskCreate();
	SPI1_DMA_Init();
	USART_DMA_Init();
	
	
	#ifdef FATFS
	
	f_mount(&fs, "0:", 1);   //挂载文件系统，文件系统挂载在SD卡上

    #ifdef DEBUG
	res=f_mount(&fs, "0:", 1);   //挂载文件系统，文件系统挂载在SD卡上

	if(res==FR_OK){
	printf("ok");
	}
    #endif

	f_open(&file, "0:data.bin", FA_CREATE_ALWAYS | FA_WRITE);  //打开文件

    #ifdef DEBUG
    res=f_open(&file, "0:data.bin", FA_CREATE_ALWAYS | FA_WRITE);  //打开文件

    
	if(res==FR_OK){
	printf("88");
	}
    #endif
		#endif
		//vUpperComputerTxSerialTaskCreate();
	//TCP_Server_Init(8080);

	//printf("LAN8720A BSP INIT AND COMFIGURE SUCCESS7845554\n");

	ADS1299_EXTI_Init();
	
	//f_write(&file, "Hello FatFs11", 20, &fnum); // 必须写数据
	//f_write(&file, "test1,test2\r\n", 12, &wlen);
	//f_close(&file);
	//f_mount(NULL,"0:",1);
	//MCO2_Config();
	//STM32F407_MCO1_PLLI2S_50M_Init();
	
	vTaskStartScheduler();
	
	
while(1){
	//SD卡写入逻辑
	#ifdef FATFS
	if(SD_num>0){
		//if(*ADC_BufA!=0|*ADC_BufB!=0){
				if (Buffer_Full_Flag == 1) {
            // 批量写入 SD 卡
            f_write(&file, (const void *)ADC_BufA, 1024, &fnum);

					#ifdef DEBUG
					printf("11");
					#endif

					Buffer_Full_Flag=0;
					SD_num--; 
				}
        else if(Buffer_Full_Flag==2){
				    f_write(&file, (const void *)ADC_BufB, 1024, &fnum);

					#ifdef DEBUG
					printf("22");
					#endif

					Buffer_Full_Flag=0;
					SD_num--;
				}
        
				if(SD_num % 10==0){
				   f_sync(&file);
				}
				
			//}
		
		}
	else if(SD_num==0){
		//f_close(&file);
	  //f_mount(NULL,"0:",1);
			f_sync(&file);

			#ifdef DEBUG
			printf("over");
			#endif
		}
		
		
#endif


/*
*			
*   if (ETH_CheckFrameReceived()){ 
     
      LwIP_Pkt_Handle();
    }
    
    LwIP_Periodic_Handle(LocalTime);
		
*/

}
}

		
	

	


/*
void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for (; nCount != 0; nCount--);
}
*/


/*********************************************END OF FILE**********************/

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName){
    /* 当检测到某个任务发生栈溢出时，程序会进入这里 */
    (void)xTask;
    (void)pcTaskName; // pcTaskName 即为爆栈的任务名字

    /* 禁用中断，进入死循环，方便打断点调试 */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

