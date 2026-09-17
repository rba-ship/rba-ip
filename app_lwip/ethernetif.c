/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "lwip/opt.h"
#include "lwip/sys.h"

/**
* @file
* Ethernet Interface Skeleton
*
*/

/*
* Copyright (c) 2001-2004 Swedish Institute of Computer Science.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
* 3. The name of the author may not be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
* SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
*
* This file is part of the lwIP TCP/IP stack.
*
* Author: Adam Dunkels <adam@sics.se>
*
*/



#if 1 /* don't build, this is only a skeleton, see previous comment */

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "LAN8742A.h"
#include "stm32f429_eth.h"
#include "ethernetif.h"
//#include "stm32f429_eth.c"
#include "string.h"



#include "FreeRTOS.h"
#include  "semphr.h"
#include "../app_semphr/app_semphr.h"
#include "app_task.h"
//#include "netif/ppp_oe.h"

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'
/* STM32F407 的 96位 UID 映射基地址 */
#define MCU_UID_BASE  0x1FFF7A10 

volatile int counteth=0;

//SemaphoreHandle_t EthSemaphore = NULL;

sys_sem_t tx_sem = {0};//NULL;
sys_mbox_t eth_tx_mb = {0};//NULL;
static struct netif *s_pxNetIf = NULL;
/* ================= 以太网 DMA 描述符及缓冲区内存分配 ================= */

/* 1. 缓冲区数量与大小定义 */
#ifndef ETH_RXBUFNB
#define ETH_RXBUFNB             4       /* 接收 DMA 描述符/缓冲区数量 */
#endif

#ifndef ETH_TXBUFNB
#define ETH_TXBUFNB             4       /* 发送 DMA 描述符/缓冲区数量 */
#endif

#ifndef ETH_RX_BUF_SIZE
#define ETH_RX_BUF_SIZE         1524    /* 每个接收缓冲区字节大小 */
#endif

#ifndef ETH_TX_BUF_SIZE
#define ETH_TX_BUF_SIZE         1524    /* 每个发送缓冲区字节大小 */
#endif

/* 2. DMA 描述符结构体数组（必须 4 字节地址对齐） */
//__align(4) ETH_DMADESCTypeDef  DMARxDescTab[ETH_RXBUFNB]; /* 接收描述符 */
//__align(4) ETH_DMADESCTypeDef  DMATxDescTab[ETH_TXBUFNB]; /* 发送描述符 */

/* 3. 真实存放网络数据包的 SRAM 缓冲区（必须 4 字节地址对齐） */
//__align(4) uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE]; /* 接收 SRAM 缓存 */
//__align(4) uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE]; /* 发送 SRAM 缓存 */

ETH_DMADESCTypeDef *DMATxDescToSet;
ETH_DMADESCTypeDef *DMARxDescToGet;

extern ETH_DMADESCTypeDef DMARxDscrTab[ETH_RXBUFNB];
extern ETH_DMADESCTypeDef DMATxDscrTab[ETH_TXBUFNB];
extern uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE];
extern uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE];
/* ====================================================================== */

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};

/* Forward declarations. */
//void  ethernetif_input(struct netif *netif);



static void generate_unique_mac(uint8_t *mac_out)
{
    // 读取 96 位 UID（包含 3 个 32 位无符号整数）
    uint32_t uid0 = *(uint32_t *)(MCU_UID_BASE + 0);
    uint32_t uid1 = *(uint32_t *)(MCU_UID_BASE + 4);
    uint32_t uid2 = *(uint32_t *)(MCU_UID_BASE + 8);

    // 将 3 个 32 位 UID 进行异或混合计算，降低碰撞概率
    uint32_t mix_hash = uid0 ^ uid1 ^ uid2;

    /* 1. 配置前 3 字节：使用 ST 意法半导体官方分配的 OUI 前缀 */
    mac_out[0] = 0x00;
    mac_out[1] = 0x80;
    mac_out[2] = 0xE1;

    /* 2. 配置后 3 字节：由芯片 UID 的哈希值动态填充 */
    mac_out[3] = (uint8_t)(mix_hash >> 16);
    mac_out[4] = (uint8_t)(mix_hash >> 8);
    mac_out[5] = (uint8_t)(mix_hash >> 0);
}




/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
void low_level_init(struct netif *netif)
{
  //struct ethernetif *ethernetif = netif->state;
  uint32_t i =0;
  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */
 // netif->hwaddr[0] = 0x00;
    //netif->hwaddr[0] = 0x00;
//...
  //netif->hwaddr[5] = ;
	generate_unique_mac(netif->hwaddr);


  /* maximum transfer unit */
  netif->mtu = 1500;
  
  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
	
	s_pxNetIf=netif;
	if(EthSemaphore == NULL){
			EthSemaphore = xSemaphoreCreateBinary();
	}
  if(sys_sem_new(&tx_sem , 0) == ERR_OK)
		#ifdef LWIP_DEBUG_ME
    printf("sys_sem_new ok\n");
    #endif
  if(sys_mbox_new(&eth_tx_mb , 50) == ERR_OK)
		#ifdef LWIP_DEBUG_ME
    printf("sys_mbox_new ok\n");
    #endif
	  //写入MAC地址
	ETH_MACAddressConfig(ETH_MAC_Address0, netif->hwaddr);
	#ifdef LWIP_DEBUG_ME
	printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
       netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2],
       netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);
	#endif
	//以太网芯片的初始化
	//ETH_BSP_Config();
		//sys_thread_new("ETHIN",
    //              ethernetif_input,  /* 任务入口函数 */
     //             netif,        	  /* 任务入口函数参数 */
     //             NETIF_IN_TASK_STACK_SIZE,/* 任务栈大小 */
      //            NETIF_IN_TASK_PRIORITY); /* 任务的优先级 */
	//
	/* 7. 初始化以太网 DMA 描述符链表 (建立硬件和内存缓冲区的映射) */
  /* 初始化发送描述符 */
  ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
  /* 初始化接收描述符 */
  ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);
	
	for(i=0; i<ETH_RXBUFNB; i++)
    {
      ETH_DMARxDescReceiveITConfig(&DMARxDscrTab[i], ENABLE);
    }
	
	#ifdef CHECKSUM_BY_HARDWARE
  /* 如果开启了硬件校验和卸载，需要为每个发送描述符开启 TCP/IP 校验和插入 */
  for(int i = 0; i < ETH_TXBUFNB; i++)
  {
    ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
  }
#endif
	
	

  /* 9. 开启以太网 MAC 和 DMA 硬件引擎 */
  ETH_Start();
  /* Do whatever else is needed to initialize interface. */  
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

err_t low_level_output(struct netif *netif, struct pbuf *p)
{
  //struct ethernetif *ethernetif = netif->state;
  struct pbuf *q;
	uint8_t *buffer;
	uint32_t framelength = 0;
	
	//__IO ETH_DMADESCTypeDef *DMARxDesc;
  //initiate transfer();
	//xSemaphoreTake(EthSemaphore, portMAX_DELAY);
  /* 1. 检查当前 DMA 发送描述符是否可用 (判断 OWN 位是否为 0) */
    if ((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (uint32_t)RESET) {
        /* DMA 正在忙着发送上一个帧，描述符还没被释放 */
				//xSemaphoreGive(EthSemaphore);
        return ERR_MEM; /* 告诉 LwIP 内存/缓冲区忙 */
    }
#if ETH_PAD_SIZE
  pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
		
		/* 2. 获取当前写有效 DMA 发送缓冲区的内存首地址 */
    buffer = (uint8_t *)(DMATxDescToSet->Buffer1Addr);

  for(q = p; q != NULL; q = q->next) {
    /* Send the data from the pbuf to the interface, one pbuf at a
       time. The size of the data in each pbuf is kept in the ->len
       variable. */
    //send data from(q->payload, q->len);
		memcpy((uint8_t *)&buffer[framelength], q->payload, q->len);
        framelength += q->len;
  }
	
	
	// 3. 设置帧长度并配置控制位（如硬件校验和、SOF/EOF 标志）
    DMATxDescToSet->ControlBufferSize = p->tot_len;
    DMATxDescToSet->Status |= ETH_DMATxDesc_FS | ETH_DMATxDesc_LS | ETH_DMATxDesc_IC;

    // 4. 将 OWN 位置 1，把控制权交给 DMA 硬件触发发送
    DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;

    // 5. 唤醒 DMA 发送（若 DMA 处于 Suspend 状态）
    if ((ETH->DMASR & ETH_DMASR_TBUS) != (uint32_t)RESET) {
        ETH->DMASR = ETH_DMASR_TBUS;
        ETH->DMATPDR = 0; // 向发送轮询寄存器写任意值
    }

    // 6. 将软件指针更新指向下一个发送描述符
    DMATxDescToSet = (ETH_DMADESCTypeDef *)(DMATxDescToSet->Buffer2NextDescAddr);
		
		
/*
		printf("DMASR   = %08X\r\n", ETH->DMASR);
		printf("DMACHTDR= %08X\r\n", ETH->DMACHTDR);
		printf("DMACHRBAR= %08X\r\n", ETH->DMACHRBAR);
		printf("DMACHTBAR= %08X\r\n", ETH->DMACHTBAR);
		*/
		//xSemaphoreGive(EthSemaphore);

//  //signal that packet should be sent();
//	/* 4. 将实际发送长度写入 DMA 描述符，并把 OWN 位置 1 (控制权交给 DMA 硬件) */
//    if (ETH_Prepare_Transmit_Descriptors(framelength) == ETH_SUCCESS) {
//        /* 5. 触发/恢复 DMA 发送引擎，将数据从内存推送到 LAN8720A 并发出 */
//       // ETH_ResumeDMA_Transmission();
//        ETH_ResumeDMATransmission();
//        /* 更新统计计数器 */
//        LINK_STATS_INC(link.xmit);
//        return ERR_OK;
//    }

   // return ERR_BUF;

#if ETH_PAD_SIZE
  pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
  
  LINK_STATS_INC(link.xmit);

  return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
struct pbuf *
low_level_input(struct netif *netif)
{
  struct pbuf *p = NULL, *q;
  u16_t len;
  uint8_t *buffer;
  uint32_t l = 0;
  uint32_t framelength;

  /* 1. Descriptor still owned by DMA -> nothing new */
  if ((DMARxDescToGet->Status & ETH_DMARxDesc_OWN) != (uint32_t)RESET) {
    return NULL;
  }

  /* 2. Error frame? just drop it and release the descriptor */
  if ((DMARxDescToGet->Status & ETH_DMARxDesc_ES) != (uint32_t)RESET) {
    goto release;
  }

  /* 3. Must be a complete frame in a single descriptor (FS+LS both set) */
  if (((DMARxDescToGet->Status & ETH_DMARxDesc_FS) == (uint32_t)RESET) ||
      ((DMARxDescToGet->Status & ETH_DMARxDesc_LS) == (uint32_t)RESET)) {
    goto release;   /* shouldn't happen with 1524-byte buffers, but bail safely */
  }

  /* 4. Frame length (minus 4-byte CRC) and buffer pointer, straight from this descriptor */
  framelength = ((DMARxDescToGet->Status & ETH_DMARxDesc_FL) >> ETH_DMARxDesc_FrameLengthShift) - 4;
  buffer = (uint8_t *)DMARxDescToGet->Buffer1Addr;
  len = framelength;

#if ETH_PAD_SIZE
  len += ETH_PAD_SIZE;
#endif

  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

  if (p != NULL) {
#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE);
#endif
    for (q = p; q != NULL; q = q->next) {
      memcpy((uint8_t *)q->payload, (uint8_t *)&buffer[l], q->len);
      l += q->len;
    }
#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE);
#endif
    LINK_STATS_INC(link.recv);
  } else {
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
  }

release:
  /* 5. Hand this descriptor back to DMA */
  DMARxDescToGet->Status = ETH_DMARxDesc_OWN;

  /* 6. Advance to next descriptor */
  DMARxDescToGet = (ETH_DMADESCTypeDef *)(DMARxDescToGet->Buffer2NextDescAddr);

  /* 7. If DMA was suspended for lack of descriptors, wake it back up */
  if ((ETH->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET) {
    ETH->DMASR = ETH_DMASR_RBUS;
    ETH->DMARPDR = 0;
  }

  return p;
}
//struct pbuf *
//low_level_input(struct netif *netif)
//{
////  struct ethernetif *ethernetif = netif->state;
//  struct pbuf *p=NULL, *q;
//  u16_t len;
//  uint8_t *buffer;
//  FrameTypeDef frame;
//  uint32_t l = 0;
//  __IO ETH_DMADESCTypeDef *DMARxDesc;
//  //uint32_t i = 0;
//	
//	// 1. 检查当前 RX 描述符 OWN 位是否归 CPU 所有 (OWN == 0 说明 DMA 已经写完数据)
//  if ((DMARxDescToGet->Status & ETH_DMARxDesc_OWN) != (uint32_t)RESET) {
//       return NULL; // 没有新数据到达
//    }
//	frame = ETH_Get_Received_Frame();
//	if (frame.length == 0) {
//    return NULL;
//  }
//	
//	if ((frame.descriptor->Status & ETH_DMARxDesc_ES) == (uint32_t)RESET) {
//  /* Obtain the size of the packet and put it into the "len"
//     variable. */
//  len = frame.length;
//  buffer = (uint8_t *)frame.buffer;
//	
//#if ETH_PAD_SIZE
//  len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
//#endif

//  /* We allocate a pbuf chain of pbufs from the pool. */
//  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
//  }
//	
//  if (p != NULL) {

//#if ETH_PAD_SIZE
//    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
//#endif

//    /* We iterate over the pbuf chain until we have read the entire
//     * packet into the pbuf. */
//    for(q = p; q != NULL; q = q->next) {
//      /* Read enough bytes to fill this pbuf in the chain. The
//       * available data in the pbuf is given by the q->len
//       * variable.
//       * This does not necessarily have to be a memcpy, you can also preallocate
//       * pbufs for a DMA-enabled MAC and after receiving truncate it to the
//       * actually received size. In this case, ensure the tot_len member of the
//       * pbuf is the sum of the chained pbuf len members.
//       */
//      //read data into(q->payload, q->len);
//			memcpy((uint8_t *)q->payload, (uint8_t *)&buffer[l], q->len);
//      l += q->len;
//    }
//		LINK_STATS_INC(link.recv);
//	}
//    //acknowledge that packet has been read();

//#if ETH_PAD_SIZE
//    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
//#endif

//    
//   else {
//    //drop packet();
//    LINK_STATS_INC(link.memerr);
//    LINK_STATS_INC(link.drop);
//  }
//		/* 把"真正被消费的那个描述符"交还给 DMA，而不是碰全局指针 */
//  frame.descriptor->Status = ETH_DMARxDesc_OWN;
//	if ((ETH->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET) {
//    ETH->DMASR = ETH_DMASR_RBUS;
//    ETH->DMARPDR = 0;
//  }
// // 4.【核心步骤】清除该描述符的错误状态，重新将 OWN 位置 1，归还给 DMA
//    //DMARxDescToGet->Status |= ETH_DMARxDesc_OWN;

//    // 5. 将软件指针更新指向下一个接收描述符
//    //DMARxDescToGet = (ETH_DMADESCTypeDef *)(DMARxDescToGet->Buffer2NextDescAddr);

//    // 6. 如果 DMA 之前因为缺乏描述符而挂起，唤醒 DMA 接收
//    //if ((ETH->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET) {
//     //   ETH->DMASR = ETH_DMASR_RBUS;
//     //   ETH->DMARPDR = 0; // 向接收轮询寄存器写任意值
//    //}
//  return p;  
//}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void ethernetif_input( void * pvParameters)
{
  //struct ethernetif *ethernetif;
	#ifdef LWIP_DEBUG_ME
	printf("ETH_IN Task Running\r\n");
  #endif
  struct eth_hdr *ethhdr;
  struct pbuf *p =NULL;

  //ethernetif = netif->state;
	while(1){
			//if (xSemaphoreTake(EthSemaphore, portMAX_DELAY) == pdTRUE){
				/* move received packet into a new pbuf */
				p = low_level_input(s_pxNetIf);
				/* no packet could be read, silently ignore this */
				if (p == NULL) return;
				/* points to packet payload, which starts with an Ethernet header */
				ethhdr = p->payload;
		#ifdef LWIP_DEBUG_ME
			printf("RX dst=%02X:%02X:%02X:%02X:%02X:%02X type=0x%04X\r\n",
       ethhdr->dest.addr[0], ethhdr->dest.addr[1], ethhdr->dest.addr[2],
       ethhdr->dest.addr[3], ethhdr->dest.addr[4], ethhdr->dest.addr[5],
       htons(ethhdr->type));
    #endif
			//switch (htons(ethhdr->type)) {
			/* IP or ARP packet? */
			//case ETHTYPE_IP:
			//case ETHTYPE_ARP:
#if PPPOE_SUPPORT
  /* PPPoE packet? */
  case ETHTYPE_PPPOEDISC:
  case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
    /* full packet send to tcpip_thread to process */
    if (s_pxNetIf->input(p, s_pxNetIf)!=ERR_OK){
					LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
			 printf("netif->input FAILED\r\n");   // <-- is this ever printed?
					pbuf_free(p);
					p = NULL;
		}
	//}
    //break;

    //default:
    //pbuf_free(p);
    //p = NULL;
    //break;

		}
  }
//}



//void ethernetif_input(void *pParams) {
//	struct netif *netif;
//	struct pbuf *p = NULL;
//	netif = (struct netif*) pParams;
//  LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
//  
//	while(1) 
//  {
//    if(xSemaphoreTake( s_xSemaphore, portMAX_DELAY ) == pdTRUE)
//    {
//      /* move received packet into a new pbuf */
//      taskENTER_CRITICAL();
//      TRY_GET_NEXT_FRAGMENT:
//      p = low_level_input(netif);
//      taskEXIT_CRITICAL();
//      /* points to packet payload, which starts with an Ethernet header */
//      if(p != NULL)
//      {
//        taskENTER_CRITICAL();
//        /* full packet send to tcpip_thread to process */
//        if (netif->input(p, netif) != ERR_OK)
//        {
//          LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
//          pbuf_free(p);
//          p = NULL;
//        }
//        else
//        {
//          xSemaphoreTake( s_xSemaphore, 0);
//          goto TRY_GET_NEXT_FRAGMENT;
//        }
//        taskEXIT_CRITICAL();
//      }
//    }
//	}
//}

#if !LWIP_ARP
/**
 * This function has to be completed by user in case of ARP OFF.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if ...
 */
static err_t low_level_output_arp_off(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{  
  err_t errval;
  errval = ERR_OK;
    
/* USER CODE BEGIN 5 */ 
    
/* USER CODE END 5 */  
    
  return errval;
  
}

#endif



/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init(struct netif *netif)
{
  struct ethernetif *ethernetif;

  LWIP_ASSERT("netif != NULL", (netif != NULL));
    
  //ethernetif = mem_malloc(sizeof(struct ethernetif));
  //if (ethernetif == NULL) {
  //  LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
		//vTaskDelete(NULL);
  //  return ERR_MEM;
  //}

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  //NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->state = ethernetif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;
  
  //ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);
  
  /* initialize the hardware */
  low_level_init(netif);
	
	//etharp_init();
  //(ARP_TMR_INTERVAL, arp_timer, NULL);
  	/* ==================== 【关键缺失代码】 ==================== */
    /* 使用 LwIP 的抽象接口创建一个名为 "ETHIN" 的 FreeRTOS 接收任务 */
  //sys_thread_new("ETHIN", 
   //                ethernetif_input_task,       /* 任务入口函数 */
   //                netif,                  /* 传给任务的参数 */
   //                1024,                   /* 任务堆栈深度 (建议不小于 1024) */
   //                5); /* 任务优先级 (建议设为较高优先级) */
    /* ======================================================== */

  return ERR_OK;
}

//extern xSemaphoreHandle s_xSemaphore; /* LwIP ethernetif.c 中定义的接收信号量 */

void ETH_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
		//printf("ETH IRQ");
    /* 检查是否产生了 ETH DMA 接收中断 (RBUIE / RIE) */
    if (ETH_GetDMAITStatus(ETH_DMA_IT_R) != RESET)
    {
			  counteth++;
        /* 释放二值信号量，唤醒 ethernetif_input 任务去读取 DMA 数据 */
        if (EthSemaphore != NULL) {
            xSemaphoreGiveFromISR(EthSemaphore, &xHigherPriorityTaskWoken);
        }
        
        /* 清除 ETH 接收中断标志位 */
        ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
        ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
    }
		
		if (ETH_GetDMAITStatus(ETH_DMA_IT_RBU) != RESET)
    {
        /* 清除 RBU 标志 */
        //ETH_DMAClearITPendingBit(ETH_DMA_IT_RBU);
        //ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
				ETH_DMAClearITPendingBit(ETH_DMA_IT_R | ETH_DMA_IT_RBU | ETH_DMA_IT_AIS | ETH_DMA_IT_NIS);

        /* 恢复 DMA 接收（通知 DMA 重新检查描述符） */
        ETH->DMARPDR = 0; 
    }

    /* 如果唤醒了更高优先级的任务，触发一次任务切换 */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
/*
static void arp_timer(void *arg)
{
  etharp_tmr();
  sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);
}
*/
#endif /* 0 */

