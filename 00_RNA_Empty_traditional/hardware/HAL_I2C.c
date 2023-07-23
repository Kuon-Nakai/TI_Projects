/* --COPYRIGHT--,BSD
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//****************************************************************************
//
// HAL_I2C.c - Hardware abstraction layer for I2C with MSP432P401R
//
//****************************************************************************

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "HAL_I2C.h"
#include "dma.h"

/* I2C Master Configuration Parameter */
const eUSCI_I2C_MasterConfig i2cConfig =
{
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        48000000,                               // SMCLK = 48MHz
        EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 100khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

/**
 * @brief Initializes I2C GPIO.
 * 
 * @deprecated This function is deprecated. Use GPIO_Setup.h macros instead.
 */
void Init_I2C_GPIO()
{
    /* Select I2C function for I2C_SCL(P6.5) & I2C_SDA(P6.4) */
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P6,
            GPIO_PIN5,
            GPIO_PRIMARY_MODULE_FUNCTION);

    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P6,
            GPIO_PIN4,
            GPIO_PRIMARY_MODULE_FUNCTION);
}

/**
 * @brief Starts DMA Reception of an already initialized I2C module.
 *
 * @param module        I2C module to be configured. [ EUSCI_Bx_BASE ]
 * @param DMAChPeriph   DMA channel peripheral selection [ DMA_CHx_<periph_function> ]. Type in one of them & F12 to see the full list.
 * @param DMACh         DMA channel number as a integer.
 * @param interrupt     DMA interrupt number. Valid parameters: INT_DMA_INT0, INT_DMA_INT1, INT_DMA_INT2, INT_DMA_INT3.
 * @param buf           Pointer to the buffer to be filled.
 * @param size          Size of the buffer to be filled.
 */
inline void I2C_RxStart_DMA(uint32_t module, uint32_t DMAChPeriph, uint8_t DMACh, uint8_t interrupt, void *buf, uint16_t size) {
    DMA_Init(DMAChPeriph, DMACh, UDMA_PRI_SELECT, UDMA_SIZE_8, UDMA_SRC_INC_NONE, UDMA_DST_INC_8, UDMA_ARB_1, UDMA_MODE_BASIC, interrupt, 
        (void *)MAP_I2C_getReceiveBufferAddressForDMA(module), buf, size, 0x00);

}

/**
 * @brief Configures I2C in master mode.
 * 
 * @param module I2C module to be configured. [ EUSCI_Bx_BASE ]
*/
void I2C_M_Init(uint32_t module)
{
    /* Initialize USCI_B0 and I2C Master to communicate with slave devices*/
    I2C_initMaster(module, &i2cConfig);

    /* Disable I2C module to make changes */
    I2C_disableModule(module);

    /* Enable I2C Module to start operations */
    I2C_enableModule(module);
    return;
}


/***************************************************************************//**
 * @brief  Reads data from the sensor
 * @param  writeByte Address of register to read from
 * @return Register contents
 ******************************************************************************/

int I2C_read16(uint32_t module, unsigned char writeByte)
{
    int val = 0;
    int valScratch = 0;

    /* Set master to transmit mode PL */
    I2C_setMode(module, EUSCI_B_I2C_TRANSMIT_MODE);

    /* Clear any existing interrupt flag PL */
    I2C_clearInterruptFlag(module, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

    /* Wait until ready to write PL */
    while (I2C_isBusBusy(module));

    /* Initiate start and send first character */
    I2C_masterSendMultiByteStart(module, writeByte);

    /* Wait for TX to finish */
    while(!(I2C_getInterruptStatus(module,
        EUSCI_B_I2C_TRANSMIT_INTERRUPT0)));

    /* Initiate stop only */
    I2C_masterSendMultiByteStop(module);

    /* Wait for Stop to finish */
    while(!I2C_getInterruptStatus(module, EUSCI_B_I2C_STOP_INTERRUPT));

    /*
     * Generate Start condition and set it to receive mode.
     * This sends out the slave address and continues to read
     * until you issue a STOP
     */
    I2C_masterReceiveStart(module);

    /* Wait for RX buffer to fill */
    while(!(I2C_getInterruptStatus(module, EUSCI_B_I2C_RECEIVE_INTERRUPT0)));

    /* Read from I2C RX register */
    val = I2C_masterReceiveMultiByteNext(module);

    /* Receive second byte then send STOP condition */
    valScratch = I2C_masterReceiveMultiByteFinish(module);

    /* Shift val to top MSB */
    val = (val << 8);

    /* Read from I2C RX Register and write to LSB of val */
    val |= valScratch;

    /* Return temperature value */
    return (int16_t)val;
}


/***************************************************************************//**
 * @brief  Writes data to the sensor
 * @param  regAddr  Address of register you want to modify
 * @param  writeByte Data to be written to the specified register
 * @return none
 ******************************************************************************/
void I2C_write16(uint32_t module, unsigned char regAddr, unsigned int writeByte)
{
    /* Set master to transmit mode PL */
    I2C_setMode(module, EUSCI_B_I2C_TRANSMIT_MODE);

    /* Clear any existing interrupt flag PL */
    I2C_clearInterruptFlag(module, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

    /* Wait until ready to write PL */
    while (I2C_isBusBusy(module));

    /* Initiate start and send first character */
    I2C_masterSendMultiByteStart(module, regAddr);

    /* Send the MSB to SENSOR */
    I2C_masterSendMultiByteNext(module, (unsigned char)(writeByte>>8));
    I2C_masterSendMultiByteFinish(module, (unsigned char)(writeByte&0xFF));
}

void I2C_setslave(uint32_t module, unsigned int slaveAdr)
{
    /* Specify slave address for I2C */
    I2C_setSlaveAddress(module, slaveAdr);

    /* Enable and clear the interrupt flag */
    I2C_clearInterruptFlag(module, EUSCI_B_I2C_TRANSMIT_INTERRUPT0 + EUSCI_B_I2C_RECEIVE_INTERRUPT0);
    return;
}
