/****************************************************/
// MSP432P401R
// 配置Keil独立空工程 (已将ti固件库打包至工程)
// Keil工程已配置开启FPU
// Bilibili：m-RNA
// E-mail:m-RNA@qq.com
// 创建日期:2021/9/28
/****************************************************/

#include "sysinit.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "tim32.h"
#include "key.h"
#include "timA.h"
#include <string.h>
#include <stdlib.h>
#include "servo.h"

#define ENABLE_CMD_ECHO     0
#define ENABLE_SERVO2_CMD   1

extern UART_RxCtrl rc_a0;

ServoControl2 *sc;
char cmd[32];

void cb(uint8_t *data, uint8_t len);

int main(void)
{
    SysInit();         // 第3讲 时钟配置
    uart_init_IT(115200); // 第7讲 串口配置
    delay_init();      // 第4讲 滴答延时

    uart_A0_RxToZero(cb, true); // Configure debug probe UART, with auto buffer expansion
    /*开始填充初始化代码*/
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
    KEY_Init(1);
    // TimA0_Int_Init(47999, TIMER_A_CLOCKSOURCE_DIVIDER_64);
    // TimA1_PWM_Init(65535, TIMER_A_CLOCKSOURCE_DIVIDER_64, 300);

    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4 | GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);
    sc = ServoControl2_init(50, TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, TIMER_A_CAPTURECOMPARE_REGISTER_2);
    // ServoControl2_setParams(sc, 1500, 2500, 11.111111f, -11.666667f);

    /*停止填充初始化代码*/

    printf("Hello,MSP432!\r\n");
    MAP_Interrupt_enableMaster(); // 开启总中断
    while (1)
    {
        /*开始填充用户代码*/
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        delay_ms(500);
        /*停止填充用户代码*/
    }
}

void EUSCIA0_IRQHandler(){
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG){
        uint8_t data = MAP_UART_receiveData(EUSCI_A0_BASE);
        // Handle incoming UART transmission
        // UART_transmitData(EUSCI_A0_BASE, data);
        rxHandler(&rc_a0, data);
    }
}

// UART A0 Rx callback for command processing
void cb(uint8_t *data, uint8_t len)
{
#if ENABLE_CMD_ECHO
    printf("Received %d bytes: ", len);
    printf("%s\n", data);
#endif
    if(strcmp((char *)data, "/") == 0){
        memset(cmd, 0, 32);
        uart_A0_RxReload();
        return;
    }
#if ENABLE_SERVO2_CMD
    else if (strncasecmp(cmd, "servo2", 6) == 0 || strncasecmp((char *)data, "servo2", 6) == 0) {
        if      (strncasecmp((char *)data + 7, "azi ", 4) == 0)     ServoControl2_setAzimuth(sc, atof((char *)data + 11));
        else if (strncasecmp((char *)data + 7, "ele ", 4) == 0)     ServoControl2_setElevation(sc, atof((char *)data + 11));
        else if (strncasecmp((char *)data + 7, "cal0", 4) == 0)     ServoControl2_calibrate0(sc);
        else if (strncasecmp((char *)data + 7, "cal90", 5) == 0)    ServoControl2_calibrate90(sc);
    }
#endif
    else printf("Unknown command: %s\n", data);
    uart_A0_RxReload();
}
