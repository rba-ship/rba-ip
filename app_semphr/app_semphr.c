#include "app_semphr.h"


//该信号量用于以太网全局中断，使得程序在初始化的时候接入ethernetif_input函数，完成lwip的初始化，
SemaphoreHandle_t EthSemaphore = NULL; /* 以太网接收信号量 */

/**
 * @brief 初始化网卡接收二值信号量，目前创建的是动态创建的，请修改为静态创建的。
 
 */
void Ethernetif_Semaphore_Create(void)
{
    if (EthSemaphore == NULL)
    {
        /* 创建二值信号量 */
        EthSemaphore = xSemaphoreCreateBinary();
        
        /* 校验信号量是否创建成功 */
        configASSERT(EthSemaphore != NULL);
    }
}
