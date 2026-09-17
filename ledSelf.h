/******************************************************
*这是LED自写程序
* 
*
*******************************************************/


#include"stm32f4xx.h"
//管脚宏定义
//红色
#define LED1_PIN           GPIO_Pin_6
#define LED1_GPIO_PORT     GPIOF
#define LED1_GPIO_CLK      RCC_AHB1Periph_GPIOF

//绿色
#define LED2_PIN           GPIO_Pin_7
#define LED2_GPIO_PORT     GPIOF
#define LED2_GPIO_CLK      RCC_AHB1Periph_GPIOF


//蓝色
#define LED3_PIN           GPIO_Pin_8
#define LED3_GPIO_PORT     GPIOF
#define LED3_GPIO_CLK      RCC_AHB1Periph_GPIOF

//设置LED的亮灭
#define ON     0
#define OFF    1

//设置三个函数。它们完成LED的引脚设置，其实质是设置LED灯的亮灭
#define      LED1(a)    if(a){GPIO_SetBits(LED1_GPIO_PORT, LED1_PIN);}\
else { GPIO_ResetBits(LED1_GPIO_PORT, LED1_PIN); }
#define	     LED2(a)    if(a)  GPIO_SetBits(LED2_GPIO_PORT, LED2_PIN);\
else  GPIO_ResetBits(LED2_GPIO_PORT, LED2_PIN);
#define	     LED3(a)    if(a)  GPIO_SetBits(LED3_GPIO_PORT, LED3_PIN);\
else  GPIO_ResetBits(LED3_GPIO_PORT, LED3_PIN);

//注意到，要想配置引脚的高低电平，必须配置寄存器的的高低电平，我们查阅手册，（pp190）配置的寄存器就是ODR寄存器.
//但是，配置ODR寄存器又只能通过配置BSRR寄存器来达到配置BSRR寄存器的目的，因此，我们继续编写宏，使得我们可以更方便的控制寄存器。

//这里，我们的宏定义完成置位与复位的功能，实质上就是通过配置BSRR寄存器来控制ODR寄存器
#define      BSRR_SET_HIGH(m,n)     {m->BSRRL=n;}
#define      BSRR_SET_LOW(m,n)      {m->BSRRH=n;}
#define      BSRR_TOGGLE(m,n)       {m->ODR^=n;}

//进一步宏
#define      LED1_ON       BSRR_SET_LOW(LED1_GPIO_PORT,LED1_PIN)
#define      LED1_OFF      BSRR_SET_HIGH(LED1_GPIO_PORT,LED1_PIN)
#define      LED1_TOGGLE   BSRR_TOGGLE(LED1_GPIO_PORT,LED1_PIN)

#define      LED2_ON       BSRR_SET_LOW(LED2_GPIO_PORT,LED2_PIN)
#define      LED2_OFF      BSRR_SET_HIGH(LED2_GPIO_PORT,LED2_PIN)
#define      LED2_TOGGLE   BSRR_TOGGLE(LED2_GPIO_PORT,LED2_PIN)


#define      LED3_ON       BSRR_SET_LOW(LED3_GPIO_PORT,LED3_PIN)
#define      LED3_OFF      BSRR_SET_HIGH(LED3_GPIO_PORT,LED3_PIN)
#define      LED3_TOGGLE   BSRR_TOGGLE(LED3_GPIO_PORT,LED3_PIN)

//红
#define LED_RED  \
					LED1_ON;\
					LED2_OFF;\
					LED3_OFF

//绿
#define LED_GREEN		\
					LED1_OFF;\
					LED2_ON;\
					LED3_OFF

//蓝
#define LED_BLUE	\
					LED1_OFF;\
					LED2_OFF;\
					LED3_ON


//黄(红+绿)					
#define LED_YELLOW	\
					LED1_ON;\
					LED2_ON;\
					LED3_OFF
//紫(红+蓝)
#define LED_PURPLE	\
					LED1_ON;\
					LED2_OFF;\
					LED3_ON

//青(绿+蓝)
#define LED_CYAN \
					LED1_OFF;\
					LED2_ON;\
					LED3_ON

//白(红+绿+蓝)
#define LED_WHITE	\
					LED1_ON;\
					LED2_ON;\
					LED3_ON

//黑(全部关闭)
#define LED_RGBOFF	\
					LED1_OFF;\
					LED2_OFF;\
					LED3_OFF		




void gpioLedConfig(void);






