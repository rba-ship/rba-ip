#include"stm32f4xx.h"

extern volatile uint8_t DATA_BUFFER[27];

/* ADS1299内部寄存器地址定义	*/
#define ADS1299_REG_ID            0x00        // ID Control Register : The ID Control Register is programmed during device manufacture to indicate device characteristics
#define ADS1299_REG_CONFIG1       0x01        // Configuration Register 1
#define ADS1299_REG_CONFIG2       0x02        // Configuration Register 2
#define ADS1299_REG_CONFIG3       0x03        // Configuration Register 3
#define ADS1299_REG_LOFF          0x04        // Lead-Off Control Register
#define ADS1299_REG_CH1SET        0x05        // The CH[1]SET Control Register configures the power mode, PGA gain, and multiplexer settings channels
#define ADS1299_REG_CH2SET        0x06        // The CH[2]SET Control Register configures the power mode, PGA gain, and multiplexer settings channels
#define ADS1299_REG_CH3SET        0x07        // The CH[3]SET Control Register configures the power mode, PGA gain, and multiplexer settings channels
#define ADS1299_REG_CH4SET        0x08        // The CH[4]SET Control Register configures the power mode, PGA gain, and multiplexer settings channels
#define ADS1299_REG_CH5SET        0x09        // The CH[5]SET Control Register configures the power mode, PGA gain, and multiplexer settings channels
#define ADS1299_REG_CH6SET        0x0A        // The CH[6]SET Control Register configures the power mode, PGA gain, and multiplexer settings channels
#define ADS1299_REG_CH7SET        0x0B        // The CH[7]SET Control Register configures the power mode, PGA gain, and multiplexer settings channels
#define ADS1299_REG_CH8SET        0x0C        // The CH[8]SET Control Register configures the power mode, PGA gain, and multiplexer settings channels
#define ADS1299_REG_RLD_SENSP     0x0D        // Controls the selection of the positive signals from each channel for right leg drive derivation
#define ADS1299_REG_RLD_SENSN     0x0E        // Controls the selection of the negative signals from each channel for right leg drive derivation
#define ADS1299_REG_LOFF_SENSP    0x0F        // Selects the positive side from each channel for lead-off detection
#define ADS1299_REG_LOFF_SENSN    0x10        // Selects the negative side from each channel for lead-off detection
#define ADS1299_REG_LOFF_FLIP     0x11        // Controls the direction of the current used for lead-off derivation
#define ADS1299_REG_LOFF_STATP    0x12        // Stores the status of whether the positive electrode on each channel is on or off (Read-Only Register)
#define ADS1299_REG_LOFF_STATN    0x13        // Stores the status of whether the negative electrode on each channel is on or off (Read-Only Register)
#define ADS1299_REG_GPIO          0x14        // General-Purpose I/O Register: controls the action of the three GPIO pins
#define ADS1299_REG_PACE          0x15        // PACE Detect Register: configure the channel signal used to feed the external PACE detect circuitry
#define ADS1299_REG_RESP          0x16        // Respiration Control Register: provides the controls for the respiration circuitry
#define ADS1299_REG_CONFIG4       0x17        // Configuration Register 4: 
//#define ADS1299_REG_WCT1          0x18        // Wilson Central Terminal and Augmented Lead Control Register
//#define ADS1299_REG_WCT2          0x19        // Wilson Central Terminal Control Register


/*ADS1299命令定义*/
/*系统命令*/
#define ADS1299_WAKEUP	        	0X02	//从待机模式唤醒
#define ADS1299_STANDBY	        0X04	//进入待机模式
#define ADS1299_ADSRESET        	0X06	//复位
#define ADS1299_START	        	0X08	//启动或转换
#define ADS1299_STOP	        		0X0A	//停止转换
#define ADS1299_OFFSETCAL				0X1A	//通道偏移校准
/*数据读取命令*/
#define ADS1299_RDATAC	        	0X10	//启用连续的数据读取模式,默认使用此模式
#define ADS1299_SDATAC	        	0X11	//停止连续的数据读取模式
#define ADS1299_RDATA	        	0X12	//通过命令读取数据;支持多种读回。
/*寄存器读取命令*/
//请注意传送命令需要将寄存器的地址与它相与
#define	ADS1299_RREG	        		0X20	//读取001r rrrr 000n nnnn  
#define ADS1299_WREG	        		0X40	//写入010r rrrr 000n nnnn

/*	r rrrr=要读、写的寄存器地址
		n nnnn=要读、写的数据		*/

#define ADS1299_REG_ONE  0x00  //f发送第二个命令，即发送的寄存器个数-1，一般取0x00即可


/*               */
/*寄存器内部值定义 */
/*              */



//CONIG1:Configuration Register 1||daisy_en,clock,datarate||reset value=0x96
//bit  7   6			5		 4    3		2     1      0
//     1   daisy_en		clk_en	 1	  0		(DR				)
//10010110
#define CONFIG1_RESET      0x96
#define CONFIG1_RESERVED   0x90//请将本值与定义的值相与
#define DAISY_MODE            (0<<6)
#define MULTIPLE_MODE            (1<<6)
#define DATA_RATE_16K    0x00 
#define DATA_RATE_8K     0x01
#define DATA_RATE_4K	 0x02
#define DATA_RATE_2K 0x03
#define DATA_RATE_1K 0x04
#define DATA_RATE_500 0x05
#define DATA_RATE_250 0x06

//CONIG2:Configuration Register 2||test_signal_genneration||reset value=0xc0
//bit  7   6		5		 4           3		       2             1      0
//     1   1		0 	     INT_CAL	0            CAL_AMP		(CAL_FREQ)
#define CONFIG2_RESET  0xC0
#define CONFIG2_RESERVED  0xC0
#define TEST_SOURSE_EX    0x00
#define TEST_SOURSE_IN    0x10   
#define TEST_AMP_1        0x00
#define TEST_AMP_2        0x04
#define TEST_FREQ_21      0x00
#define TEST_FREQ_20      0x01
#define TEST_DC           0x03

//CONIG3:Configuration Register 3||internal OR EXTERNAL reerence||reset value=0x60
//bit  7           6		5		 4           3		        2             1               0
//     PD_REFBUF   1		1 	     BIAS_MEAS	 BIASREF_INT    PD_BIAS		BIAS_LOFF_SENS    BIAS_STAT
#define CONFIG3_RESET  0x60
#define CONFIG3_RESERVED  0x60

#define PD_REFBUF_DIS 0x00        
#define PD_REFBUF_EN  0x80  
#define BIAS_MEAS_DIS 0x00        
#define BIAS_MEAS_EN  0x10        
#define BIASREF_INT_EX  0x00        
#define BIASREF_INT_IN  0x08      
#define PD_BIAS_DIS    0x00        
#define PD_BIAS_EN     0x04        
#define BIAS_SENS_DIS  0x00        
#define BIAS_SENS_EN   0x02        
#define BIAS_EN			0x00        
#define BIAS_DIS		0x01      

//LOFF:Lead-OFF Control Register||Lead-OFF detection||reset value=0x00
//bit  7           6		5		 4           3		        2             1               0
//     (COMP_TH2               ) 	 0	         (ILEAD_OFF       )		     (FLEAD-OFF        )
#define LOFF_RESET       0x00
#define LOFF_RESERVED    0x00

//CHSnET:Individual Channel setting Register,N=1:8||POWER_MODE PGA MUX||reset value=0x61
//bit  7      6		5		 4           3		        2             1               0
//     PDn	 (gainN            )	   SRB2	            (MUXn                          )
#define CHnSET_RESET 0x61
#define CHnSET_RESERVED 0x00
#define PDn_EN    0<<7
#define PDn_DIS    1<<7

#define GAIN_1     0x00
#define GAIN_2     0x10
#define GAIN_4     0x20
#define GAIN_8     0x40
#define GAIN_12    0x50
#define GAIN_24    0x60

#define SRB2_DIS     0<<3
#define SRB2_EN     1<<3

#define MUXn_NORMAL     0
#define MUXn_SHORT     1
#define MUXn_TEM       4
#define MUXn_TEST       5

//BIAS_SENSP:Bias Drive Postive Derivation Register||POWER_MODE PGA MUX||reset value=0x00
//bit  7      6		5		 4           3		        2             1               0
//    BIASP8  BIASP7 BIASP6 BIASP5      BIASP4      BIASP3       BIASP2   BIASP1
#define BIAS_SENSP_RESET 0x00


//BIAS_SENSN:Bias Drive Negtive Derivation Register||POWER_MODE PGA MUX||reset value=0x00
//bit  7      6		5		 4           3		        2             1               0
//    BIASN8  BIASN7 BIASN6 BIASN5      BIASN4      BIASN3       BIASN2   BIASN1
#define BIAS_SENSN_RESET 0x00

//LOFF_SENSP:Bias Drive Negtive Derivation Register||POWER_MODE PGA MUX||reset value=0x00
//bit  7      6		5		 4           3		        2             1               0
//    BIASN8  BIASN7 BIASN6 BIASN5      BIASN4      BIASN3       BIASN2   BIASN1
#define LOFF_SENSP_RESET 0x00

//LOFF_SENSN:Bias Drive Negtive Derivation Register||POWER_MODE PGA MUX||reset value=0x00
//bit  7      6		5		 4           3		        2             1               0
//    BIASN8  BIASN7 BIASN6 BIASN5      BIASN4      BIASN3       BIASN2   BIASN1
#define BIAS_SENSN_RESET 0x00

//LOFF_FLIP:Bias Drive Negtive Derivation Register||POWER_MODE PGA MUX||reset value=0x00
//bit  7      6		5		 4           3		        2             1               0
//    BIASN8  BIASN7 BIASN6 BIASN5      BIASN4      BIASN3       BIASN2   BIASN1
#define LOFF_FLIP_RESET 0x00

//GPIO:GPIO Register||POWER_MODE PGA MUX||reset value=0x0F
//bit  7      6		5		 4           3		        2             1               0
//    BIASN8  BIASN7 BIASN6 BIASN5      BIASN4      BIASN3       BIASN2   BIASN1
#define GPIO_RESET 0x0F

//MISC1:Bias Drive Negtive Derivation Register||POWER_MODE PGA MUX||reset value=0x00
//bit  7      6		5		 4           3		        2             1               0
//    BIASN8  BIASN7 BIASN6 BIASN5      BIASN4      BIASN3       BIASN2   BIASN1
#define MISC1_RESET 0x00

//MISC1:Bias Drive Negtive Derivation Register||POWER_MODE PGA MUX||reset value=0x00
//bit  7      6		5		 4           3		        2             1               0
//    BIASN8  BIASN7 BIASN6 BIASN5      BIASN4      BIASN3       BIASN2   BIASN1
#define MISC2_RESET 0x00

//CONFIG4:Bias Drive Negtive Derivation Register||POWER_MODE PGA MUX||reset value=0x00
//bit  7      6		5		 4           3		        2             1               0
//    BIASN8  BIASN7 BIASN6 BIASN5      BIASN4      BIASN3       BIASN2   BIASN1
#define CONFIG4_RESET 0x00



void ADS1299_SPI_Init(void);
uint8_t SPI_ReadWriteByte(uint8_t);
void ADS1299_SendCmd(uint8_t);
void ADS1299_WriteReg(uint8_t addr, uint8_t val);
//void ADS1299_WRITE_REG(int registerAddress);
uint8_t ADS1299_ReadReg(uint8_t);
void ADS1299_Init(void);
uint8_t ADS1299_Get_DATA(void);
void ADS1299_Power_UP(void);
double ADS1299_Voltage_TRANSFER(uint8_t *);
void ADS1299_Get_DATA_DMA(void);
