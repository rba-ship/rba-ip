
#ifndef __APP_QUEUE_H
#define __APP_QUEUE_H

#include "../FreeRTOS/FreeRTOS.h"
#include "../FreeRTOS/queue.h"
#include "Data_Pack.h"



#define QUEUE_LENGTH_4     4 
#define QUEUE_LENGTH_1024  1024
#define ITEM_SIZE       sizeof( uint8_t* ) 

//1024字节的queue创建句柄
extern QueueHandle_t g_xADS1299Queue;

//2字节的传递内存指针的队列
extern QueueHandle_t xDmaPointerQueue;

extern QueueHandle_t xUartTxQueue;

/*
void dataQueueCreate(void);
*/

//1024字节队列的创建函数，返回值为QueueHandle_t句柄
QueueHandle_t dataQueueCreate(void);


//2字节的队列的初始化函数，主要是创建一个队列用于传递指向queue的一个指针
void APP_pointerQueue_Init(void);

//1024字节队列的初始化函数，主要是创建两个用于数据打包的队列。
void APP_dataQueue_Init(void);


void CreatexDmaPointerQueue( void);
void  Createg_xADS1299Queue(void);

//xUartTxQueue = xQueueCreate(2, sizeof(Pack1024_t *));

void CreatexUartTxQueue( void );

#endif

