#include "lwip/tcpip.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "ethernetif.h"
#include "lwip/sockets.h"
#include "lwip/priv/sockets_priv.h"
#include "etharp.h"
#include "LAN8742A.h"
#include "Data_Pack.h"
#define LINK_TIMER_INTERVAL        1000

#define TCP_TMR_INTERVAL       250  /* The TCP timer interval in milliseconds. */
uint32_t TCPTimer = 0;
uint32_t ARPTimer = 0;
uint32_t LinkTimer = 0;
uint32_t IPaddress = 0;

/* 定义全局网卡结构体 */
struct netif gnetif;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;

/**
  * @brief  tcpip_init 初始化完成后的回调函数
  * @note   此函数会自动在 LwIP 的核心线程 (tcpip_thread) 上下文中执行
  */
static void lwip_init_done_callback(void* arg)
{
		//printf("callback Ok\n");
	//struct netif gnetif;

    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;

    /* 1. 设置静态 IP 地址、子网掩码、默认网关 */
    IP4_ADDR(&ipaddr, 192, 168, 1, 100);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 1, 1);

    /* 2. 调用原生 API 注册网卡 */
    netif_add(&gnetif,
        &ipaddr,
        &netmask,
        &gw,
        NULL,
        &ethernetif_init,  /* 传入底层硬件驱动初始化接口 */
        &tcpip_input);     /* OS 模式必须使用 tcpip_input 投递数据包 */
	//printf("netif Ok");
	
	//printf("LAN8720A Ethernet Demo\n");
  
	//printf("ping实验例程\n");
	
	//printf("使用同一个局域网中的电脑ping开发板的地址，可进行测试\n");

	//IP地址和端口可在netconf.h文件修改
  //printf("本地IP和端口: %d.%d.%d.%d\n",IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
  //printf("IP Address: %s\r\n", ip4addr_ntoa(&ipaddr));
	/* Configure ethernet (GPIOs, clocks, MAC, DMA) */
  //ETH_BSP_Config();	
  //printf("LAN8720A BSP INIT AND COMFIGURE SUCCESS7845554\n");

    /* 3. 设置为默认网卡并启动 */
    netif_set_default(&gnetif);
   // netif_set_up(&gnetif);

    /* (可选) 如果在 lwipopts.h 中开启了 LWIP_NETIF_LINK_CALLBACK */
   // netif_set_link_up(&gnetif);
	 if (netif_is_link_up(&gnetif))
  {
    /* When the netif is fully configured this function must be called */
		//LOCK_TCPIP_CORE();
    netif_set_up(&gnetif);
		//UNLOCK_TCPIP_CORE();
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
  }
}

/**
  * @brief  应用层调用的 LwIP 启动入口
  */
void native_lwip_init(void)
{
   struct netif *p_netif;    /* 调用原生 tcpip_init，并传入初始化完成后的回调函数 */
	#ifdef LWIP_DEBUG_ME
	  printf("before tcpip_init 1\r\n");
	#endif
    //tcpip_init(lwip_init_done_callback, NULL);
	  tcpip_init(NULL, NULL);
	  //vTaskDelay(pdMS_TO_TICKS(200)); /* 给 tcpip_thread 时间去执行 sys_mutex_new(&mem_mutex) */
	  
	#ifdef LWIP_DEBUG_ME
		printf("after tcpip_init\r\n");
		printf("callback Ok\n");
  #endif

    /* 1. 设置静态 IP 地址、子网掩码、默认网关 */
    IP4_ADDR(&ipaddr, 192, 168, 1, 241);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 1, 1);
	#ifdef LWIP_DEBUG_ME
		printf("IP Address: %s\r\n", ip4addr_ntoa(&ipaddr));
	#endif
    /* 2. 调用原生 API 注册网卡 */
  p_netif=netif_add(&gnetif,
        &ipaddr,
        &netmask,
        &gw,
        NULL,
        &ethernetif_init,  /* 传入底层硬件驱动初始化接口 */
        &tcpip_input);  
  #ifdef LWIP_DEBUG_ME				
	printf("netif Ok,");
	#endif
	if (p_netif == NULL) 
		{
			#ifdef LWIP_DEBU_ME
    printf("Error: netif_add failed!\r\n");
			#endif
    // 如果这里打印了失败，说明底层 ethernetif_init 初始化没过，不能继续执行后续 netif 函数！
    return; 
}
		printf("本地IP地址是:%d.%d.%d.%d\n\n",  \
        ((gnetif.ip_addr.addr)&0x000000ff),       \
        (((gnetif.ip_addr.addr)&0x0000ff00)>>8),  \
        (((gnetif.ip_addr.addr)&0x00ff0000)>>16), \
        ((gnetif.ip_addr.addr)&0xff000000)>>24);
		#ifdef LWIP_DEBUG_ME
	 printf("本地IP地址是:%d.%d.%d.%d\n\n",  \
        ((gnetif.ip_addr.addr)&0x000000ff),       \
        (((gnetif.ip_addr.addr)&0x0000ff00)>>8),  \
        (((gnetif.ip_addr.addr)&0x00ff0000)>>16), \
        ((gnetif.ip_addr.addr)&0xff000000)>>24);
   #endif

	//printf("LAN8720A Ethernet Demo\n");
  
	//printf("ping实验例程\n");
	
	//printf("使用同一个局域网中的电脑ping开发板的地址，可进行测试\n");

	//IP地址和端口可在netconf.h文件修改
  //printf("本地IP和端口: %d.%d.%d.%d\n",IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
  
	/* Configure ethernet (GPIOs, clocks, MAC, DMA) */
  //ETH_BSP_Config();	
  //printf("LAN8720A BSP INIT AND COMFIGURE SUCCESS7845554\n");

    /* 3. 设置为默认网卡并启动 */
    netif_set_default(&gnetif);
		#ifdef LWIP_DEBUG_ME
		printf("gnetif address: 0x%p\r\n", (void *)&gnetif);
		#endif
		if (netif_is_link_up(&gnetif))
  {
		#ifdef LWIP_DEBUG_ME
		printf("Link is UP, setting netif up...\r\n");
		#endif
    /* When the netif is fully configured this function must be called */
    netif_set_up(&gnetif);
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
  }
    //netif_set_up(&gnetif);

    /* (可选) 如果在 lwipopts.h 中开启了 LWIP_NETIF_LINK_CALLBACK */
    //netif_set_link_up(&gnetif);
	  //lwip_init_done_callback((void*)1);
	  //printf("LwIP TCPIP Thread initialized successfully!\r\n");

}


/* net_app.c */
int g_server_fd = -1;

/**
 * @brief 初始化 TCP 服务器监听套接字
 */
int TCP_Server_Init(uint16_t port)
{
    struct sockaddr_in server_addr;
	#ifdef LWIP_DEBUG_ME
		printf("FreeRAM: %d\r\n", xPortGetFreeHeapSize());
		printf("\r\n===== BEFORE SOCKET =====\r\n");

		printf("FreeRTOS heap = %u\r\n",
       (unsigned int)xPortGetFreeHeapSize());

		printf("NETCONN used = %u\r\n",
       (unsigned int)lwip_stats.memp[MEMP_NETCONN]->used);

		printf("NETCONN max  = %u\r\n",
       (unsigned int)lwip_stats.memp[MEMP_NETCONN]->max);

		printf("NETCONN err  = %u\r\n",
       (unsigned int)lwip_stats.memp[MEMP_NETCONN]->err);
	
	    /* 0. 打印编译进来的真实配置值（防止混版树配置错位） */
    printf("MEMP_NUM_NETCONN = %d\r\n", MEMP_NUM_NETCONN);
    printf("LWIP_SOCKET = %d, NO_SYS = %d\r\n", LWIP_SOCKET, NO_SYS);
    printf("TCP_RECVMBOX_SIZE = %d\r\n", DEFAULT_TCP_RECVMBOX_SIZE);
   #endif 
    /* 1. 裸测 netconn 池 */
    void *m = memp_malloc(MEMP_NETCONN);
		#ifdef LWIP_DEBUG_ME
    printf("probe1 memp_malloc = %p\r\n", m);
		#endif
    if (m) { memp_free(MEMP_NETCONN, m); }

    /* 2. 测 netconn 创建（含 mbox/sem） */
    //struct netconn *c = netconn_new(NETCONN_TCP);
    //printf("probe2 netconn_new = %p\r\n", (void *)c);
    //if (c) { netconn_delete(c); }

    /* 3. 正确的统计读取方式 */
#if LWIP_STATS && MEMP_STATS && LWIP_DEBUG_ME
    printf("pool used=%u max=%u err=%u\r\n",
           lwip_stats.memp[MEMP_NETCONN]->used,
           lwip_stats.memp[MEMP_NETCONN]->max,
           lwip_stats.memp[MEMP_NETCONN]->err);
#endif
		
		
    /* 1. 创建 TCP Socket */
    g_server_fd = socket(AF_INET, SOCK_STREAM, 0);
		#ifdef LWIP_DEBUG_ME
	  printf("[LWIP DEBUG] NUM_SOCKETS = %d\n", NUM_SOCKETS);
	 //printf("NETCONN free: %d\r\n", memp_free_count(MEMP_NETCONN));
    //printf("TCP_PCB free: %d\r\n", memp_free_count(MEMP_TCP_PCB));
    //printf("PBUF free: %d\r\n", memp_free_count(MEMP_PBUF_POOL));

		printf("\r\n===== AFTER SOCKET =====\r\n");

printf("server_fd = %d\r\n", g_server_fd);
printf("errno     = %d\r\n", errno);

printf("NETCONN used = %u\r\n",
       (unsigned int)lwip_stats.memp[MEMP_NETCONN]->used);

printf("NETCONN max  = %u\r\n",
       (unsigned int)lwip_stats.memp[MEMP_NETCONN]->max);

printf("NETCONN err  = %u\r\n",
       (unsigned int)lwip_stats.memp[MEMP_NETCONN]->err);
			 #endif
	  //stats_display();
    if (g_server_fd < 0){ 
			  //stats_display(); /* 串口会打印出所有的 memp 内存池使用情况 */
			  printf("Socket 创建失败，errno = %d\r\n", errno);
				//printf("socket 创建失败，errno = %d\r\n", errno);
				return -1;
		}
    //if (g_server_fd>0)
    /* 2. 绑定本地 IP 与端口 */
		#ifdef LWIP_DEBUG_ME
		printf("Socket 创建成功！fd = %d\r\n", g_server_fd);
		#endif
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);               /* 大端字节序转换 */
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(g_server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        lwip_close(g_server_fd);
        g_server_fd = -1;
        return -1;
    }

    /* 3. 开启监听 */
    if (listen(g_server_fd, 5) < 0) {
        lwip_close(g_server_fd);
        g_server_fd = -1;
        return -1;
    }
		
		send(g_server_fd,test,strlen(test),0);

		#ifdef LWIP_DEBUG_ME
    printf("TCP Server initialized on port %d   tcp_server \r\n", port);
		#endif
    return 0;
}

void LwIP_Pkt_Handle(void)
{
  /* Read a received packet from the Ethernet buffers and send it to the lwIP for handling */
  ethernetif_input(&gnetif);
}

void LwIP_Periodic_Handle(__IO uint32_t localtime)
{
#if LWIP_TCP
  /* TCP periodic process every 250 ms */
  if (localtime - TCPTimer >= TCP_TMR_INTERVAL)
  {
    TCPTimer =  localtime;
    tcp_tmr();
  }
	 
  /* ARP periodic process every 5s */
  if ((localtime - ARPTimer) >= ARP_TMR_INTERVAL)
  {
    ARPTimer =  localtime;
    etharp_tmr();
  }
	
	/* Check link status periodically */
	if ((localtime - LinkTimer) >= LINK_TIMER_INTERVAL) {
		ETH_CheckLinkStatus(ETHERNET_PHY_ADDRESS);
	}
	
#ifdef USE_DHCP
  /* Fine DHCP periodic process every 500ms */
  if (localtime - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
  {
    DHCPfineTimer =  localtime;
    dhcp_fine_tmr();
    if ((DHCP_state != DHCP_ADDRESS_ASSIGNED) && 
        (DHCP_state != DHCP_TIMEOUT) &&
          (DHCP_state != DHCP_LINK_DOWN))
    {
#ifdef SERIAL_DEBUG
			LED1_TOGGLE;
			printf("\nFine DHCP periodic process every 500ms\n");
#endif /* SERIAL_DEBUG */
      
      /* process DHCP state machine */
      LwIP_DHCP_Process_Handle();
    }
  }

  /* DHCP Coarse periodic process every 60s */
  if (localtime - DHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS)
  {
    DHCPcoarseTimer =  localtime;
    dhcp_coarse_tmr();
  }

#endif

}
#endif
