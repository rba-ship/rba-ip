//#ifndef h

//#endif // 

#ifndef DATA_PACK_H
#define DATA_PACK_H

#include <stdint.h>
#include <string.h>



//定义相关的帧头帧尾，请注意在
#define FRAME_HEADER_0  0xA5
#define FRAME_HEADER_1  0x5A

#define FRAME_COUNTER   0x00

#define FRAME_CHECKSUM  0x00
#define FRAME_FOOTER    0x01


//定义常规的为数据结构，

#define RAW_BUF_SIZE  (32 * 27) // 864 字节，由SPI传输过来的ADC原始信号流，二进制代码的大小，等待进一步的打包过程。bufA和bufB的大小分别为768字节


//864字节的存储空间声明
extern  uint8_t g_dma_buf_a[RAW_BUF_SIZE]; 
extern  uint8_t g_dma_buf_b[RAW_BUF_SIZE]; 

//uint8_t *p_buf = g_dma_buf_a; // 拿到已满的 buf0 地址
//uint8_t  volatile *p_buf_a = g_dma_buf_a; // 拿到已满的 buf0 地址
//uint8_t volatile *p_buf_b = g_dma_buf_b; // 拿到已满的 buf0 地址
extern  uint8_t *p_buf_a;
extern  uint8_t *p_buf_b;

extern char test[];


//打包的数据结构定义，它们是32字节宽的数据，

// 强制 1 字节对齐，防止编译器自动填充空位破坏 32 字节的紧凑结构
//#pragma pack(push, 1)
#pragma pack(push, 1)

typedef struct __attribute__((packed)){
    uint8_t  header[2];     // 帧头：0xA5, 0x5A
    uint8_t  counter;       // 帧序号：0x00 ~ 0xFF 循环
    //uint8_t  status[3];     // ADS1299 状态字（3字节）
    uint8_t  ch_data[27];   // 8个通道数据（24字节）
    uint8_t  checksum;      // 异或校验和
    uint8_t  footer;        // 帧尾：0x01
}ADC32_Frame_t;


//_Static_assert(sizeof(ADC32_Frame_t) == 32, "ADS1299_Frame_t size must be 32 bytes!");


//1024字节大队列
typedef struct {
    ADC32_Frame_t frames[32]; // 32 * 32 = 1024 字节
} Pack1024_t;
#pragma pack(pop)

extern Pack1024_t tx_pack;

//此函完成数据存储的中转开辟
void ADCFrameCreate(ADC32_Frame_t* ADS1299_Frame_t);

#endif

