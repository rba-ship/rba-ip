#include <stdio.h>
#include "stm32f4xx.h"

#include "Data_Pack.h"

extern struct netif gnetif;

static void lwip_init_done_callback(void* arg);
void native_lwip_init(void);
int TCP_Server_Init(uint16_t port);
void LwIP_Pkt_Handle(void);
void LwIP_Periodic_Handle(__IO uint32_t localtime);
