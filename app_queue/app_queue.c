#include "app_queue.h"
#include "../Data_Pack.h"
#include "../FreeRTOS/queue.h"
#include "Data_Pack.h"


QueueHandle_t xDmaPointerQueue = NULL;  //输入给打包函数的队列，存放着指针
QueueHandle_t xUartTxQueue = NULL;     //输出给串口的队列，存放着指针


QueueHandle_t g_xADS1299Queue = NULL;

/* The variable used to hold the queue's data structure. */
static StaticQueue_t xStaticQueue;
static StaticQueue_t xStaticQueue3;

static StaticQueue_t xStaticQueue2;


uint8_t ucQueueStorageArea2[ QUEUE_LENGTH_4 * ITEM_SIZE ];
uint8_t ucQueueStorageArea22[ QUEUE_LENGTH_4 * ITEM_SIZE ];

uint8_t ucQueueStorageArea1024[ QUEUE_LENGTH_1024 * ITEM_SIZE ];





void CreatexDmaPointerQueue( void ) { 
		//QueueHandle_t xQueueLen2; 
/* Create a queue capable of containing 10 uint64_t values. */ 
		xDmaPointerQueue = xQueueCreateStatic(  QUEUE_LENGTH_4, 
																			ITEM_SIZE, 
																			ucQueueStorageArea22, 
																			&xStaticQueue ); 
/* pxQueueBuffer was not NULL so xQueue should not be NULL. */ 
		configASSERT( xDmaPointerQueue ); 
} 


void CreatexUartTxQueue( void ) { 
		//QueueHandle_t xQueueLen2; 
/* Create a queue capable of containing 10 uint64_t values. */ 
		xUartTxQueue = xQueueCreateStatic(  QUEUE_LENGTH_4, 
																			ITEM_SIZE, 
																			ucQueueStorageArea2, 
																			&xStaticQueue3 ); 
/* pxQueueBuffer was not NULL so xQueue should not be NULL. */ 
	if( xUartTxQueue == NULL ) {
    // 处理错误：例如，点亮一个LED，或者进入一个死循环以便调试
    // 程序不应该继续运行
    while(1);
	}
		configASSERT( xUartTxQueue ); 
} 
/*
QueueHandle_t g_xADS1299Queue = NULL;
*/
/*数据queue队列设计*/
//这个是无返回值的函数，不要用这个
/*
void  dataQueueCreate(void) {
// 创建可以容纳 32 个 32 字节元素的队列
    g_xADS1299Queue = xQueueCreate(32, sizeof(ADC32_Frame_t));
    
    // 安全检查：判断内存是否足够，队列是否创建成功
    if (g_xADS1299Queue == NULL) {
        // 创建失败处理（例如打印日志或进入 Error_Handler）
    }
}
*/


//2字节的指向buf的首地址的队列初始化函数


/*
void APP_pointerQueue_Init(void){
	
	xDmaPointerQueue = xQueueCreate(2, sizeof(uint8_t *));
	
	if (xDmaPointerQueue == NULL) {
        // 内存不足导致创建失败，进入错误处理
        // Error_Handler();
    }



}
*/



//正常使用的函数
/*
	待处理数据的队列创建函数，返回值：QueueHandle_t句柄。
	创建的队列长度为32*32=1024字节=1kb，用于数据打包的队列
	添加错误处理。


*/

/*
void  Createg_xADS1299Queue(void) {
    //QueueHandle_t xQueue = NULL;
    
    g_xADS1299Queue = xQueueCreateStatic(      QUEUE_LENGTH_1024,
																											ITEM_SIZE,
																					ucQueueStorageArea1024,																	 			
																									&xStaticQueue2);
	//sizeof(ADC32_Frame_t));
    ///if (xQueue == NULL) {
        // 错误处理...
   // }
    
   // return xQueue; // 将句柄返回给调用者
}

8*/







//void APP_Queue_Init(void){
		//QueueHandle_t xDmaPointerQueue = NULL;
//		g_xADS1299Queue = dataQueueCreate ();


//}



/*

//3.3. 中断服务函数 (零延迟切换)：ISR 零拷贝推送指针。当某个 Buffer 被硬件填满 768 字节后，中断触发，只把该 Buffer 的内存首地址传给 FreeRTOS 队列：C// 当 DMA 填满 buf0 (768B) 时触发
void SPI_DMA_M0_CpltCallback(DMA_HandleTypeDef *hdma) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint8_t *p_buf = g_ads_raw_buf0;

    // 将 buf0 地址塞给路由 Task
    xQueueSendFromISR(xDmaPointerQueue, &p_buf, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// 当 DMA 填满 buf1 (768B) 时触发
void SPI_DMA_M1_CpltCallback(DMA_HandleTypeDef *hdma) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint8_t *p_buf = g_ads_raw_buf1;

    // 将 buf1 地址塞给路由 Task
    xQueueSendFromISR(xDmaPointerQueue, &p_buf, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

*/

