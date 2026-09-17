#ifndef __APP_SEMPHR_H
#define __APP_SEMPHR_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

extern SemaphoreHandle_t EthSemaphore;

void Ethernetif_Semaphore_Create(void);

#endif
