#include"stm32f4xx.h"
//蜂鸣器
#define BUZZER_PIN          GPIO_Pin_7
#define BUZZER_GPIO_PORT    GPIOG
#define BUZZER_GPIO_CLK     RCC_AHB1Periph_GPIOG


//由原理图，置1时为有效，发声
#define BUZZER_ON         1
#define BUZZER_OFF        0

//封装成一个BUZZER函数
#define     BUZZER(a)    if(a){GPIO_SetBits(BUZZER_GPIO_PORT, BUZZER_PIN);}\
else { GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_PIN); }





void gpioBuzzerConfig(void);
void MCO1_Config(void);
