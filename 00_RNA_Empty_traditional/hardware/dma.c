#include "dma.h"

#if defined(__CC_ARM)
#pragma diag_suppress 161
#endif

#pragma region DMA control table
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(MSP_EXP432P401RLP_DMAControlTable, 1024)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment = 1024
#elif defined(__GNUC__)
__attribute__((aligned(1024)))
#elif defined(__CC_ARM)
__align(1024)
#endif
static DMA_ControlTable DMAControlTable[32];
#pragma endregion

inline void ADC_InitDMA(uint32_t channelPeriph, uint16_t *Buf, uint16_t Length) {
    // DMA_Init(channelPeriph, UDMA_PRI_SELECT, UDMA_SIZE_16, UDMA_SRC_INC_NONE, UDMA_DST_INC_16, UDMA_ARB_1, UDMA_MODE_BASIC, INT_DMA_INT1, (void *)&ADC14->MEM[0], (void *)Buf, Length, UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST | UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK);
}

void adc_dma_init(uint16_t *Data, uint16_t Length)
{
    /* Configuring DMA module */
    MAP_DMA_enableModule();
    MAP_DMA_setControlBase(DMAControlTable);
    //	DMA_disableChannelAttribute(DMA_CH7_ADC14, UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST | UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK);
    MAP_DMA_assignChannel(DMA_CH7_ADC14);
    MAP_DMA_setChannelControl(DMA_CH7_ADC14 | UDMA_PRI_SELECT, UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 | UDMA_ARB_1);
    MAP_DMA_setChannelTransfer(DMA_CH7_ADC14 | UDMA_PRI_SELECT, UDMA_MODE_BASIC, (void *)&ADC14->MEM[0], (void *)Data, Length);
    MAP_DMA_assignInterrupt(INT_DMA_INT1, 7);
    MAP_DMA_clearInterruptFlag(DMA_CH7_ADC14 & 0x0F);
    MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
    MAP_DMA_enableInterrupt(INT_DMA_INT1);
    MAP_DMA_enableChannel(7);
}

/**
 * @brief Basic function to initialize DMA.
 *
 * @param channelPeriph     DMA channel peripheral selection [ DMA_CHx_<periph_function> ]. Type in one of them & F12 to see the full list.
 * @param control           Indication if the primary or alternate control structure should be used. Valid parameters: UDMA_PRI_SELECT, UDMA_ALT_SELECT.
 * @param size              DMA transfer size. Valid parameters: UDMA_SIZE_8, UDMA_SIZE_16, UDMA_SIZE_32.
 * @param srcInc            DMA source address increment. Valid parameters: UDMA_SRC_INC_8, UDMA_SRC_INC_16, UDMA_SRC_INC_32, UDMA_SRC_INC_NONE.
 * @param dstInc            DMA destination address increment. Valid parameters: UDMA_DST_INC_8, UDMA_DST_INC_16, UDMA_DST_INC_32, UDMA_DST_INC_NONE.
 * @param arbitrationSize   DMA arbitration size, indicating how many transfers occur before an rearbitration. Valid parameters: UDMA_ARB_1, UDMA_ARB_2, UDMA_ARB_4, UDMA_ARB_8, 
 *                          UDMA_ARB_16, UDMA_ARB_32, UDMA_ARB_64, UDMA_ARB_128,  UDMA_ARB_256.
 * @param mode              DMA transfer mode. Valid parameters: UDMA_MODE_STOP, UDMA_MODE_BASIC, UDMA_MODE_AUTO, UDMA_MODE_PINGPONG, UDMA_MODE_MEM_SCATTER_GATHER, 
 *                          UDMA_MODE_PER_SCATTER_GATHER.
 * @param interrupt         DMA interrupt number. Valid parameters: INT_DMA_INT0, INT_DMA_INT1, INT_DMA_INT2, INT_DMA_INT3.
 * @param src               DMA source address.
 * @param dst               DMA destination address.
 * @param Length            DMA transfer length.
 * @param EnableAttributes  DMA channel attributes. Valid parameters: UDMA_ATTR_ALTSELECT, UDMA_ATTR_USEBURST, UDMA_ATTR_HIGH_PRIORITY, UDMA_ATTR_REQMASK.
 *                          Any attribute not included is to be explicitly disabled in this function.
 * 
 * @note Reference:
 * @note Transfer arbitration rate - slau356h 11.2.3.2 @ P628
 * @note DMA Cycle Types(modes) - slau356h 11.2.3.4 @ P630 - 639
 * 
 * @deprecated See all this parameters going like chaos here? No you don't want to use it.
 */
inline void DMA_Init(uint32_t channelPeriph, uint32_t control, uint32_t size, uint32_t srcInc, uint32_t dstInc, uint32_t arbitrationSize, uint32_t mode, uint8_t interrupt, void *src, void *dst, uint16_t Length, uint32_t enableAttributes) {
    uint8_t channel = channelPeriph & 0x0F;
    MAP_DMA_enableModule();
    MAP_DMA_setControlBase(DMAControlTable);
    MAP_DMA_assignChannel(channelPeriph);
    MAP_DMA_enableChannelAttribute(channelPeriph, enableAttributes);
    MAP_DMA_disableChannelAttribute(channelPeriph, (~enableAttributes) & 0x0F);
    MAP_DMA_setChannelControl(channelPeriph | control, size | srcInc | dstInc | arbitrationSize);
    MAP_DMA_setChannelTransfer(channelPeriph | control, mode, src, dst, Length);
    MAP_DMA_assignInterrupt(interrupt, channel);
    MAP_DMA_clearInterruptFlag(channelPeriph & 0x0F);
    MAP_Interrupt_enableInterrupt(interrupt);
    MAP_DMA_enableInterrupt(interrupt);
    MAP_DMA_enableChannel(channel);
}

void DMA_Module_Enable() {
    MAP_DMA_enableModule();
    MAP_DMA_setControlBase(DMAControlTable);
}
