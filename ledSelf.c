#include"ledSelf.h"


void  gpioLedConfig(void) {
	GPIO_InitTypeDef GPIOStruct;   //这里请参阅gpio h文件的第132行
	RCC_AHB1PeriphClockCmd(LED1_GPIO_CLK | LED2_GPIO_CLK | LED2_GPIO_CLK, ENABLE);
	GPIOStruct.GPIO_Pin = LED1_PIN;     //一下的这些，我们要继续参考手册中对这些变量的定义
	GPIOStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIOStruct.GPIO_OType = GPIO_OType_PP;
	GPIOStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIOStruct.GPIO_Speed = GPIO_Speed_2MHz;


	GPIO_Init(LED1_GPIO_PORT, &GPIOStruct);

	GPIOStruct.GPIO_Pin = LED2_PIN;
	GPIO_Init(LED2_GPIO_PORT, &GPIOStruct);

	/*选择要控制的GPIO引脚*/
	GPIOStruct.GPIO_Pin = LED3_PIN;
	GPIO_Init(LED3_GPIO_PORT, &GPIOStruct);

	/*关闭RGB灯*/
	LED_RGBOFF;



}

