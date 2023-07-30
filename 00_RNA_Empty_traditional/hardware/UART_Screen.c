#include <UART_Screen.h>

void UARTScr_MsgHandler(char *data, uint8_t size);

void UARTScr_Init()
{
    uart_init_IT(UART_SCREEN_UCA_MODULE, UART_SCREEN_BAUD);
    uart_RxLine(UART_SCREEN_RXCTRL, UARTScr_MsgHandler, true);
}

void UARTScr_Cmdf(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char *buf = (char *)malloc(UART_SCREEN_MAX_SEND_LEN);
    char *f = (char *)malloc(strlen(fmt) + 3);
    strcpy(f, fmt);
    strcat(f, "\xFF\xFF\xFF");
    vsnprintf(buf, UART_SCREEN_MAX_SEND_LEN, fmt, ap);

    uart_Txf(UART_SCREEN_UCA_MODULE, "", buf);
}

void UARTScr_MsgHandler(char *data, uint8_t size) {

}
