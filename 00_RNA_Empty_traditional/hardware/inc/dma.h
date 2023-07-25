#ifndef __DMAx_H__
#define __DMAx_H__

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

void DMA_Init(uint32_t channelPeriph, uint32_t control, uint32_t size, uint32_t srcInc, uint32_t dstInc, uint32_t arbitrationSize, uint32_t mode, uint8_t interrupt, void *src, void *dst, uint16_t Length, uint32_t enableAttributes);

#endif
