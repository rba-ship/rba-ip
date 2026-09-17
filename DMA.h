#ifndef __ADS1299_DMA_H
#define __ADS1299_DMA_H

#include "stm32f4xx.h"

// --- 宏定义 ---
#define ADS1299_DATA_LEN   27      // 3字节状态位 + 8通道*3字节 = 27
#define ADS_CS_LOW()       GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define ADS_CS_HIGH()      GPIO_SetBits(GPIOA, GPIO_Pin_4)

// --- 全局变量声明 ---
extern uint8_t g_ads_rx_buf[ADS1299_DATA_LEN]; // 接收缓冲区
extern volatile uint8_t g_data_ready_flag;      // 数据就绪标志位
extern volatile uint8_t Buffer_Full_Flag; // 1: BufA满, 2: BufB满

extern volatile uint8_t ADC_BufA[512];
extern volatile uint8_t ADC_BufB[512];

// --- 函数原型声明 ---
void ADS1299_EXTI_Init(void);
void ADS1299_DMA_Init(void);  // 初始化GPIO, SPI, DMA和EXTI
void ADS1299_Start_Capture(void);  // 开启采集
void ADS1299_Stop_Capture(void);   // 停止采集
void SPI1_DMA_Init(void);

void SPI_DMA_M0_CpltCallback(void);
void SPI_DMA_M1_CpltCallback(void);


void USART_DMA_Init(void);
void DMA2_Stream7_IRQHandler(void);

#endif
