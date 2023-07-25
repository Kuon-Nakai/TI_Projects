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
#include "PID.h"

#if defined(__GNUC__)
// #pragma clang diagnostic ignored 161
#elif defined(__CC_ARM)
#pragma diag_suppress 161 // Suppress the "unrecognized #pragma" warning, ignoring VSCode region tags for Keil. AC5 only.
#endif

#define ENABLE_CMD_ECHO     0
#define ENABLE_SERVO2_CMD   1
#define ENABLE_PID_CMD      1

extern UART_RxCtrl rc_a0;

ServoControl2 *sc;
PIDController *pidx;
PIDController *pidy;
PIDController *pidxv;
PIDController *pidyv;
char cmd[32];

void cb(uint8_t *data, uint8_t len);

int main(void)
{
    SysInit();         // 第3讲 时钟配置

    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    uart_init_IT(EUSCI_A0_BASE, 115200); // 第7讲 串口配置
    delay_init();      // 第4讲 滴答延时

    uart_RxLine(&rc_a0, cb, true); // Configure debug probe UART, with auto buffer expansion
    /*开始填充初始化代码*/
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0); // P1.0 as GPIO output (red LED), def. 1
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2); // P2.2 as GPIO output (RGB LED B), def. 0
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
    KEY_Init(1);
    // TimA0_Int_Init(47999, TIMER_A_CLOCKSOURCE_DIVIDER_64);
    // TimA1_PWM_Init(65535, TIMER_A_CLOCKSOURCE_DIVIDER_64, 300);

    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4 | GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION); // P2.4, P2.5 as PWM output
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
        // UART_transmitData(EUSCI_A0_BASE, data);ser
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
        uart_RxReload(&rc_a0);
        return;
    }
#if ENABLE_SERVO2_CMD // servo2 <command> <value>
    else if (/*strncasecmp(cmd, "servo2", 6) == 0 ||*/ strncasecmp((char *)data, "servo2", 6) == 0) {
        if      (strncasecmp((char *)data + 7, "azi ",  4) == 0)    ServoControl2_setAzimuth(sc, atof((char *)data + 11));
        else if (strncasecmp((char *)data + 7, "ele ",  4) == 0)    ServoControl2_setElevation(sc, atof((char *)data + 11));
        else if (strncasecmp((char *)data + 7, "cal0",  4) == 0)    ServoControl2_calibrate0(sc);
        else if (strncasecmp((char *)data + 7, "cal90", 5) == 0)    ServoControl2_calibrate90(sc);
        else printf("Unknown SERVO2 command: %s\n", data);
    }
#endif
#if ENABLE_PID_CMD // pid <__: instance selection> <command> <value> 
    else if(strncasecmp((char *)data, "pid ", 4) == 0){
        PIDController *pc = NULL;
        int offset;
        if      (strncasecmp((char *)data + 4, "x ",  2) == 0)      pc = pidx,  offset = 6;
        else if (strncasecmp((char *)data + 4, "y ",  2) == 0)      pc = pidy,  offset = 6;
        else if (strncasecmp((char *)data + 4, "xv ", 2) == 0)      pc = pidxv, offset = 7;
        else if (strncasecmp((char *)data + 4, "yv ", 2) == 0)      pc = pidyv, offset = 7;
        if(pc){
            if      (strncasecmp((char *)data + offset, "kp ",      3) == 0)    pc->Kp         = atof((char *)data + offset + 3);
            else if (strncasecmp((char *)data + offset, "ki ",      3) == 0)    pc->Ki         = atof((char *)data + offset + 3);
            else if (strncasecmp((char *)data + offset, "kd ",      3) == 0)    pc->Kd         = atof((char *)data + offset + 3);
            else if (strncasecmp((char *)data + offset, "tau ",     4) == 0)    pc->tau        = atof((char *)data + offset + 4);
            else if (strncasecmp((char *)data + offset, "minout ",  7) == 0)    pc->limMin     = atof((char *)data + offset + 7);
            else if (strncasecmp((char *)data + offset, "maxout ",  7) == 0)    pc->limMax     = atof((char *)data + offset + 7);
            else if (strncasecmp((char *)data + offset, "minint ",  7) == 0)    pc->limMaxInt  = atof((char *)data + offset + 7);
            else if (strncasecmp((char *)data + offset, "maxint ",  7) == 0)    pc->limMinInt  = atof((char *)data + offset + 7);
            else if (strncasecmp((char *)data + offset, "t ",       2) == 0)    pc->T          = atof((char *)data + offset + 2);
            else {
                printf("Unknown PID command: %s\n", data);
                uart_RxReload(&rc_a0);
                return;
            }
            printf("Operation completed.\n");
        }
        else printf("PID command requires instance selection: pid <__: instance selection> <command> <value>\n");
    }
#endif
    else printf("Unknown command: %s\n", data);
    uart_RxReload(&rc_a0);
}
