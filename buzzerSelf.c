#include"buzzerSelf.h"

void gpioBuzzerConfig(void) {
	GPIO_InitTypeDef GPIOStruct;   //这里请参阅gpio h文件的第132行
	RCC_AHB1PeriphClockCmd(BUZZER_GPIO_CLK |RCC_AHB1Periph_GPIOF  , ENABLE);
	GPIOStruct.GPIO_Pin = BUZZER_PIN;     //一下的这些，我们要继续参考手册中对这些变量的定义
	GPIOStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIOStruct.GPIO_OType = GPIO_OType_PP;
	GPIOStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIOStruct.GPIO_Speed = GPIO_Speed_2MHz;


	GPIO_Init(BUZZER_GPIO_PORT, &GPIOStruct);
	
	GPIOStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOF,&GPIOStruct);
	GPIOStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOF,&GPIOStruct);
	
	 GPIO_SetBits(GPIOF, GPIO_Pin_6);
	 GPIO_SetBits(GPIOF, GPIO_Pin_7);

}
void MCO2_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1. 开启 GPIOC 的时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // 2. 配置 PC9 引脚为：复用模式、高速、推挽、上拉
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      // 复用模式
    GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;  // 高速
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    // 推挽
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      // 上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. 将 PA8 连接到 MCO1 复用功能
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_MCO);

    // 4. 选择 MCO1 的输出源为 HSE (25MHz)，分频系数选择 1 分频
    // 注意：如果你想看它是否起振，建议先选 RCC_MCO1Source_HSE
    // 如果测不到，可以改成 RCC_MCO1Source_HSI 看看 PA8 硬件通不通
    //RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
	  
		
		//RCC_PLLI2SConfig(200, 2);
		//RCC_PLLI2SCmd(ENABLE);
		//while (RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == RESET);

    // 选择 MCO1 的时钟源为 PLLI2SCLK，并进行 2 分频
    // 最终输出：100MHz / 2 = 50MHz
    //RCC_MCO2Config(RCC_MCO2Source_PLLI2SCLK, RCC_MCO2Div_2);

}

void STM32F407_MCO1_PLLI2S_50M_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;         
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置 PLLI2S (N=200, R=2)
    RCC_PLLI2SConfig(200, 2); 

    RCC_PLLI2SCmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == RESET);

    // ==========================================
    // 替代报错行的寄存器操作，强制切换 MCO1 源为 PLLI2S
    RCC->CFGR |= (1 << 23); 
    // ==========================================

    // 打开 MCO1 总闸并 2 分频，输出 50MHz
    RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_2);
}
