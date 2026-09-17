#include"ADS1299.h"
#include"buzzerSelf.h"
#include "delay.h"
#include"bsp_usart.h"
/*
引脚定义：采用的是硬件SPI！！请注意
V2:
PA4-----SPI1_NSS
PA5=----SPI1_SCK
PA6-----SPI1_MISO
PA7-----SPI1_MOSI

V3:
PA4-----SPI1_NSS
PA5=----SPI1_SCK
PA6-----SPI1_MISO
PB5-----SPI1_MOSI

*/



#define V3


volatile uint8_t DATA_BUFFER[27]={0};

volatile int i;
volatile uint8_t ID=0;
#ifdef DEBUG
volatile uint8_t config3=0;
volatile uint8_t config2=0;
volatile uint8_t config1=0;
volatile uint8_t chnset=0;
#endif

//CS引脚定义
static void ADS_CS_Low(void) { GPIO_ResetBits(GPIOA, GPIO_Pin_4); }
static void ADS_CS_High(void) { GPIO_SetBits(GPIOA, GPIO_Pin_4); }


//SPI初始化（含SPI及其对应的GPIO口）
void ADS1299_SPI_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
	  //GPIO_InitTypeDef GPIO_InitStructure1;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    // PA5,6,7 复用为 SPI1 [cite: 155]
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	  #ifdef V2
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
	  #endif
	
		#ifdef V3
	  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);
	  #endif

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		#ifdef V2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		#endif
		
		#ifdef V3
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		#endif

    // 片选、复位、启动等控制引脚
	  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; // CS
    GPIO_Init(GPIOA, &GPIO_InitStructure);


   // GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_OUT;
		//GPIOStruct.GPIO_OType = GPIO_OType_PP;
		#ifdef V2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; // RESET
    GPIO_Init(GPIOC, &GPIO_InitStructure);
		#endif
		#ifdef V3
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; // RESET
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		#endif

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // START
    GPIO_Init(GPIOB, &GPIO_InitStructure);

		#ifdef V2
    // DRDY 输入引脚   PC1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
		#endif
		
		#ifdef V3
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		#endif

    // SPI1 配置: ADS1299 要求 CPOL=0, CPHA=1 (模式1) 
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
		SPI_InitStructure.SPI_CRCPolynomial = 7;                             // CRC 多项式（默认）
    // ADS1299 fCLK=2.048MHz, SCLK上限约20MHz，STM32主频84MHz下选分频 [cite: 598, 173]
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI1, &SPI_InitStructure);
		//SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
    SPI_Cmd(SPI1, ENABLE);
		
		
    //for (/*volatile int8*/ i = 0; i < 1000; i++);
	  //Delay_us(1000);
	
	  #ifdef V2
    GPIO_SetBits(GPIOC, GPIO_Pin_5);
		#endif
		
		#ifdef V3
		GPIO_SetBits(GPIOB, GPIO_Pin_8);
		#endif
		Delay_us(1000);
}

//SPI读取数据
uint8_t SPI_ReadWriteByte(uint8_t txData) {
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, txData);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	  //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
	  //Delay_us(2);
    return (uint8_t)SPI_I2S_ReceiveData(SPI1);
}

//SPI发送命令
void ADS1299_SendCmd(uint8_t cmd) {
    ADS_CS_Low();
    SPI_ReadWriteByte(cmd);
    // 某些命令需要等待 4 tCLK 解码 [cite: 200]
    for (/*volatile int*/ i = 0; i < 100; i++);
    ADS_CS_High();
	//BUZZER(BUZZER_ON);
}

// 写寄存器 [cite: 198]
void ADS1299_WriteReg(uint8_t addr, uint8_t val) {
	  //uint8_t i=0;
    ADS_CS_Low();
    SPI_ReadWriteByte(ADS1299_WREG | addr);
	  Delay_us(2);
    SPI_ReadWriteByte(ADS1299_REG_ONE); // 写入 1 个寄存器 (n-1) [cite: 189]
	  Delay_us(2);
    SPI_ReadWriteByte(val);
	  Delay_us(2);
    ADS_CS_High();
		//BUZZER(BUZZER_ON);
}

// 读寄存器 [cite: 198]
uint8_t ADS1299_ReadReg(uint8_t addr) {
    uint8_t res;
    ADS_CS_Low();
    SPI_ReadWriteByte(ADS1299_RREG | addr);
    SPI_ReadWriteByte(0x00);
    res = SPI_ReadWriteByte(0x00);
    ADS_CS_High();
	  return res;
	  
}


void ADS1299_Init(void) {
    //ADS1299_SPI_Init();
	  //BUZZER(BUZZER_ON);
	  //ADS1299_Power_UP();
	  Delay_us(10000);
	  //ADS1299_Power_UP();
	  //printf("ADS1299 Powering");
	
	  
	
	  
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	Delay_us(10000);
	  //start引脚始终为低电平
		#ifdef  V2
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);
		#endif
		
		#ifdef  V3
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);
		#endif
		
		Delay_us(1000);

    // 硬件复位
		#ifdef V2
    GPIO_ResetBits(GPIOC, GPIO_Pin_5);
		#endif 
		#ifdef V3
		GPIO_ResetBits(GPIOB, GPIO_Pin_8);
		#endif
		
    //for (/*volatile int8*/ i = 0; i < 1000; i++);
	  Delay_us(1000);
		
		#ifdef V2
    GPIO_ResetBits(GPIOC, GPIO_Pin_5);
		#endif 
		#ifdef V3
		GPIO_SetBits(GPIOB, GPIO_Pin_8);
		#endif
		
		Delay_us(10000);
		Delay_us(10000);
		
    ADS1299_SendCmd(ADS1299_SDATAC); // 停止默认的连续读模式以配置寄存器 [cite: 198]
	
    //ADS1299_SendCmd(ADS1299_ADSRESET);
    //for (/*volatile int*/ i = 0; i < 10000; i++); // 等待稳定 [cite: 384]
	  Delay_us(10000);
		Delay_us(10000);
		Delay_us(10000);

    
	  //BUZZER(BUZZER_ON);

    //ADS1299自身配置
    ADS1299_WriteReg(ADS1299_REG_CONFIG1,CONFIG1_RESERVED | DATA_RATE_250);
		Delay_us(1000);
    ADS1299_WriteReg(ADS1299_REG_CONFIG2,CONFIG2_RESERVED | TEST_SOURSE_IN);//CONFIG2_RESERVED | TEST_SOURSE_IN);
		Delay_us(1000);
    ADS1299_WriteReg(ADS1299_REG_CONFIG3,CONFIG3_RESERVED | PD_REFBUF_EN |BIASREF_INT_IN | PD_BIAS_EN);
		Delay_us(1000);

    //一下设置表示adc的输入短接，是作为测试来使用的z
    /*
    ADS1299_WriteReg(ADS1299_REG_CH1SET, CHnSET_RESERVED | MUXn_SHORT);
		*/
    
    ADS1299_WriteReg(ADS1299_REG_CH2SET, CHnSET_RESERVED | MUXn_NORMAL | GAIN_1 );//|PDn_DIS);
		Delay_us(1000);
    ADS1299_WriteReg(ADS1299_REG_CH3SET, CHnSET_RESERVED | MUXn_NORMAL | GAIN_1 );//|PDn_DIS);
		Delay_us(1000);
    ADS1299_WriteReg(ADS1299_REG_CH4SET, CHnSET_RESERVED | MUXn_NORMAL | GAIN_1 );//|PDn_DIS);
		Delay_us(1000);
    ADS1299_WriteReg(ADS1299_REG_CH5SET, CHnSET_RESERVED | MUXn_NORMAL | GAIN_1);//|PDn_DIS);
		Delay_us(1000);
    ADS1299_WriteReg(ADS1299_REG_CH6SET, CHnSET_RESERVED | MUXn_NORMAL | GAIN_1);//|PDn_DIS);
		Delay_us(1000);
    ADS1299_WriteReg(ADS1299_REG_CH7SET, CHnSET_RESERVED | MUXn_NORMAL | GAIN_1 );//|PDn_DIS);
		Delay_us(1000);
    ADS1299_WriteReg(ADS1299_REG_CH8SET, CHnSET_RESERVED | MUXn_NORMAL | GAIN_1);//|PDn_DIS);
		Delay_us(1000);
    

    //以下为正常
    ADS1299_WriteReg(ADS1299_REG_CH1SET, CHnSET_RESERVED | MUXn_NORMAL |GAIN_1);//|PDn_DIS);
		Delay_us(30000);
    
    //ADS1299_WriteReg(ADS1299_REG_CH2SET, CHnSET_RESERVED | MUXn_NORMAL);
    //ADS1299_WriteReg(ADS1299_REG_CH3SET, CHnSET_RESERVED | MUXn_NORMAL);
    //ADS1299_WriteReg(ADS1299_REG_CH4SET, CHnSET_RESERVED | MUXn_NORMAL);
    //ADS1299_WriteReg(ADS1299_REG_CH5SET, CHnSET_RESERVED | MUXn_NORMAL);
    //ADS1299_WriteReg(ADS1299_REG_CH6SET, CHnSET_RESERVED | MUXn_NORMAL);
    //ADS1299_WriteReg(ADS1299_REG_CH7SET, CHnSET_RESERVED | MUXn_NORMAL);
    //ADS1299_WriteReg(ADS1299_REG_CH8SET, CHnSET_RESERVED | MUXn_NORMAL);
		
	ID=ADS1299_ReadReg(ADS1299_REG_ID);
				//Delay_us(3);
		//printf("ID:%x\n",ID);
    if(ID==0x3E){
				GPIO_SetBits(GPIOF,GPIO_Pin_7);
			  GPIO_ResetBits(GPIOF,GPIO_Pin_7);
			  //printf("ADS1299 Read Register Succuessfully!!!\n");
				
		}
		else{
			 GPIO_SetBits(GPIOF,GPIO_Pin_6);
  		 GPIO_ResetBits(GPIOF,GPIO_Pin_6);
			  //printf("ADS1299 Read Register faild!!!!\n");
			//BUZZER(BUZZER_ON);
		}
		
		Delay_us(30000);
		#ifdef DEBUG
		config3=ADS1299_ReadReg(ADS1299_REG_CONFIG3);
		config2=ADS1299_ReadReg(ADS1299_REG_CONFIG2);
		config1=ADS1299_ReadReg(ADS1299_REG_CONFIG1);
		chnset=ADS1299_ReadReg(ADS1299_REG_CH1SET);
		#endif
		
		/*
    GPIO_SetBits(GPIOB, GPIO_Pin_0);

    
		Delay_us(30000);
		
		
		
		//Delay_us(5);

    //
		*/
		//ADS1299_SendCmd(ADS1299_START);
		GPIO_SetBits(GPIOB, GPIO_Pin_0);
        ADS1299_SendCmd(ADS1299_RDATAC);
    //一下的为AI所写
    // 示例配置：启用内部参考电压 (4.5V) [cite: 24, 320]
    //ADS1299_WriteReg(ADS1299_REG_CONFIG3, 0xE0);
    // 设置数据采样率 [cite: 1 01]
    //ADS1299_WriteReg(ADS1299_REG_CONFIG1, 0x96); // 500 SPS

    //ADS1299_SendCmd(ADS1299_RDATAC); // 恢复连续读模式 [cite: 118]
    //GPIO_SetBits(GPIOB, GPIO_Pin_0); // START引脚拉高启动转换 [cite: 88]
}	
		
//这个函数是不使用DMA的数据获取函数，请谨慎使用
uint8_t ADS1299_Get_DATA(){
	  

		ADS_CS_Low();
	
		DATA_BUFFER[0] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[1] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[2] = SPI_ReadWriteByte(0x00);	
		
		//INPUT1
		DATA_BUFFER[3] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[4] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[5] = SPI_ReadWriteByte(0x00);
		
		//INPUT2
		DATA_BUFFER[6] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[7] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[8] = SPI_ReadWriteByte(0x00);
		
		//Delay_us(1);
		//INPUT3
		DATA_BUFFER[9] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[10] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[11] = SPI_ReadWriteByte(0x00);
		
		//INPUT4
		DATA_BUFFER[12] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[13] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[14] = SPI_ReadWriteByte(0x00);
		
		//INPIUT5
		DATA_BUFFER[15] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[16] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[17] = SPI_ReadWriteByte(0x00);
		
		//INPUT6
		DATA_BUFFER[18] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[19] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[20] = SPI_ReadWriteByte(0x00);
		
		//INPUT7
		DATA_BUFFER[21] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[22] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[23] = SPI_ReadWriteByte(0x00);
	
		//INPUT8
		DATA_BUFFER[24] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[25] = SPI_ReadWriteByte(0x00);
		DATA_BUFFER[26] = SPI_ReadWriteByte(0x00);

		ADS_CS_High();
	
	    Delay_us(1);
		
	return DATA_BUFFER[3];
}

/*
double  ADS1299_Voltage_TRANSFER(uint8_t &DATA_BUFFRE[i]){
	  int32_t adc_code;

    // 组合成 24 位补码
    adc_code = (DATA_BUFFRE[0] << 16) | (DATA_BUFFRE[1] << 8) | DATA_BUFFRE[2]);
		if((i&(0x800000))==0x800000){
				//他是负数
			i=(i-8388608)*536.442f;
		}
		else{
			i=i*536.442f;
		}
		
		return i;

}
*/

//这个是通过DMA进行数据传输的数据获取函数，目前采用此函数。
void ADS1299_Get_DATA_DMA(){

        ADS_CS_Low();
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
			
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
			
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
			
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
			
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
			
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
			
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
			
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
			
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
			SPI1->DR = 0x00;
			
			while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
			ADS_CS_High();

	    Delay_us(1);
		
	//return DATA_BUFFER[3];
}


