#ifndef __GPIO_SETUP_H__
#define __GPIO_SETUP_H__

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#define ENABLE_AUTO_SETUP   1 // If enabled, the GPIO pins will be configured automatically whenever the init functions are called.
#define ENABLE_DEBUG        0 // If enabled, the program will try to print debug messages to the console when GPIO config is done. Does nothing if UCA0 is not initialized.

#if ENABLE_AUTO_SETUP

#define GPIO_LED1_Init() \
    GPIO_setAsOutputPin(1, 0x0001) //P1.0

#define GPIO_LED2_Init() \
    GPIO_setAsOutputPin(2, 0x0001); \
    GPIO_setAsOutputPin(2, 0x0002); \
    GPIO_setAsOutputPin(2, 0x0004) // P2.0, P2.1, P2.2

#define GPIO_UCA0_UART_Init() \
    GPIO_setAsPeripheralModuleFunctionInputPin(1, 0x0004, 1) \
    GPIO_setAsPeripheralModuleFunctionOutputPin(1, 0x0008, 1) // P1.2 RX, P1.3 TX

#define GPIO_UCA1_UART_Init() \
    GPIO_setAsPeripheralModuleFunctionInputPin(2, 0x0004, 1) \
    GPIO_setAsPeripheralModuleFunctionOutputPin(2, 0x0008, 1) // P2.2 RX, P2.3 TX

#define GPIO_UCA2_UART_Init() \
    GPIO_setAsPeripheralModuleFunctionInputPin(3, 0x0004, 1) \
    GPIO_setAsPeripheralModuleFunctionOutputPin(3, 0x0008, 1) // P3.2 RX, P3.3 TX

#define GPIO_UCB0_I2C_Init() \
    GPIO_setAsPeripheralModuleFunctionInputPin(1, 0x0020, 1) \
    GPIO_setAsPeripheralModuleFunctionInputPin(1, 0x0060, 1) // P1.6 SDA, P1.7 SCL

#endif // ENABLE_AUTO_SETUP

#endif // __GPIO_SETUP_H__
