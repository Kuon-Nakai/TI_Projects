#ifndef __UART_SCREEN_H__
#define __UART_SCREEN_H__

#include "usart.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

//////////////////////// UART Screen Module Configuration ////////////////////////

#define UART_SCREEN_UCA_MODULE      EUSCI_A2_BASE   // The UART module to use.
#define UART_SCREEN_RXCTRL          &rc_a2          // RxCtrl structure of the UART lib to use.
#define UART_SCREEN_BAUD            115200          // Baud rate for the UART module.

#define UART_SCREEN_MAX_SEND_LEN    64              // Max send buffer size for the UART module.

extern UART_RxCtrl rc_a0;
extern UART_RxCtrl rc_a1;
extern UART_RxCtrl rc_a2;
extern UART_RxCtrl rc_a3;

#endif
