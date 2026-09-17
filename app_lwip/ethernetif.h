#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

static void generate_unique_mac(uint8_t *mac_addr);

extern volatile int counteth;

err_t ethernetif_init(struct netif *netif);
//err_t ethernetif_init(struct netif *netif);
//err_t ethernetif_input(struct netif *netif);
struct pbuf *low_level_input(struct netif *netif);
//void ethernetif_input(struct netif *netif);
void ethernetif_input(void *pvParams);

void low_level_init(struct netif *netif);
err_t low_level_output(struct netif *netif, struct pbuf *p);
struct pbuf * low_level_input(struct netif *netif);
#endif 
