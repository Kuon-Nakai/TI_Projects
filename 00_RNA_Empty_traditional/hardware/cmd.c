#include "cmd.h"

void CMD_cb(char *data, uint16_t size);

void Cmd_init() {
    #if CMD_BAUD_UCA0
    uart_init_IT(EUSCI_A0_BASE, CMD_BAUD_UCA0);
    uart_RxLine(&rc_a0, CMD_cb, true);
    #endif
    #if CMD_BAUD_UCA1
    uart_init_IT(EUSCI_A1_BASE, CMD_BAUD_UCA1);
    #endif
    #if CMD_BAUD_UCA2
    uart_init_IT(EUSCI_A2_BASE, CMD_BAUD_UCA2);
    #endif
    #if CMD_BAUD_UCA3
    uart_init_IT(EUSCI_A3_BASE, CMD_BAUD_UCA3);
    #endif
    #if CMD_BAUD_UCA0 || CMD_BAUD_UCA1 || CMD_BAUD_UCA2 || CMD_BAUD_UCA3
    CmdList = (Cmd_t *)malloc(sizeof(Cmd_t) * 16);
    #endif
}

// ISR
// Note: If you're using CMD module to handle UART, do NOT use any other UART based module to handle the same UART port or write your own ISR for the same UART port.
#if CMD_BAUD_UCA0
extern UART_RxCtrl rc_a0;

void EUSCIA0_IRQHandler()
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
    if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        uint8_t data = MAP_UART_receiveData(EUSCI_A0_BASE);
        // Handle incoming UART transmission
        rxHandler(&rc_a0, data);
    }
}

void CMD_cb(char *data, uint16_t size) {
    
}

#endif
