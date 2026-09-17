#include "Data_Pack.h"


//bufA和bufB的定义，他们的大小均为864字节
uint8_t g_dma_buf_a[RAW_BUF_SIZE] __attribute__((aligned(4)));
uint8_t g_dma_buf_b[RAW_BUF_SIZE] __attribute__((aligned(4)));

uint8_t *p_buf_a = g_dma_buf_a;
uint8_t *p_buf_b = g_dma_buf_b;

//uint8_t *p_buf_a = g_dma_buf_a; // 拿到已满的 buf0 地址
//uint8_t *p_buf_b = g_dma_buf_b; // 拿到已满的 buf0 地址
Pack1024_t tx_pack;

char test[]= "Hello World, Hello Ethernet";


void ADCFrameCreate(ADC32_Frame_t* Frame_t) {
	//ADC32_Frame_t* Frame_t = &ADS1299_Frame;
	//Frame_t = &ADS1299_Frame;

    static  uint8_t ADS1299_counter = 0;

	Frame_t->header[0] = FRAME_HEADER_0;
	Frame_t->header[1] = FRAME_HEADER_1;
	Frame_t->counter = ADS1299_counter++;

	Frame_t->checksum = FRAME_CHECKSUM;
	Frame_t->footer = FRAME_FOOTER;

}

//static ADC32_Frame_t ADS1299_Frame_t;

