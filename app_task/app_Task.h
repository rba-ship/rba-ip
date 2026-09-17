
#include "../FreeRTOS/FreeRTOS.h"
#include "../FreeRTOS/task.h"
#include "../FreeRTOS/queue.h"
#include "../app_queue/app_queue.h"
#include "string.h"

//内存空间栈的定义，最右侧一个数字位代表4byte
//宏定义的数字代表着创建出多少字节的内存空间
#define  STACK_SIZE_128 32
#define  STACK_SIZE_256 64
#define  STACK_SIZE_512 128
#define  STACK_SIZE_1024 256
#define  STACK_SIZE_2048 512
#define  STACK_SIZE_4096 1024
#define  STACK_SIZE_8192 2048
#define  STACK_SIZE_16384 4096

//优先级定义，和arm架构的优先级定义不一样，最高优先级为4，
#define PRI_0   0
#define PRI_1   1
#define PRI_2   2
#define PRI_3   3
#define PRI_4   4
#define PRI_5   5 
#define PRI_6   6 
#define PRI_7   7 
#define PRI_8   8 
#define PRI_9   9



extern TaskHandle_t Task1;
extern TaskHandle_t uartTask;
extern TaskHandle_t TcpTask;


/*任务函数定义*/
//串口传输至上位机的任务
void vUpperComputerTxSerialTask(void *pvParameters);
void vUpperComputerTxSerialTaskCreate(void);


//数据打包函数：主要功能为将ADS1299传输过来的数据加上数据帧，防止数据错误
//单独的创建函数，如果需要这个任务，请调用create函数，无需调用task函数，本任务已做解耦
void vDataRouterTask(void *pvParameters);
void RouterTaskCreate(void);
//void vTask(void* pvParameters);


void vUpperComputerTxTcpTask(void *pvParameters);
void TcpTxTaskCreate(void);

void ethernetif_input_task(void *argument);
void EthInTaskCreate(void);

void AppTask(void *argument);
void AppTaskCreate(void);

void TcpTestTask(void *pvParameters);
void TcpTestTaskCreate(void);