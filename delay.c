#include "delay.h"

void Delay_us_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    // 1. 开启 TIM2 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 

    // 2. 配置定时器：假设系统主频 72MHz
    // 预分频器 (Prescaler) 设为 72-1，则定时器频率 = 72MHz / 72 = 1MHz (即 1us 计数一次)
    TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1; 
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF; // 自动重装载值设为最大
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	  TIM_SetCounter(TIM2, 0);


    // 3. 启动定时器
    TIM_Cmd(TIM2, ENABLE);
}

void Delay_us(uint16_t nus)
{
    //uint16_t start = TIM_GetCounter(TIM2); // 获取当前计数值
    
    // 循环等待，直到差值达到指定的微秒数
    // 使用 uint16_t 的差值计算可以自动处理计数器溢出的情况
    //while ((uint16_t)(TIM_GetCounter(TIM2) - start) < nus);
	  TIM_SetCounter(TIM2, 0);

    // 等待计数到达 nus
    while(TIM_GetCounter(TIM2) < nus);
}
