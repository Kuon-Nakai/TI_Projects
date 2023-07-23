/****************************************************/
// MSP432P401R
// 串口配置
// Bilibili：m-RNA
// E-mail:m-RNA@qq.com
/****************************************************/

/******************   版本更新说明   *****************
 * 
 * CCS支持printf
 * Keil支持标准C库跟微库
 * 用Keil开发终于可以不开微库啦
 * 
 * ? 需要注意：
 * ①使用标准C库时，将无法使用scanf。
 * 如果需要使用scanf时，请使用微库 MicroLIB
 * ①低频时钟频率下，高波特率使得传输时误差过大,
 * 比如35768Hz下19200波特率,
 * 会使得传输出错，这时可以尝试降低波特率。
 * ②baudrate_calculate的问题请去文件内查看。
 * 
 * **************************************************
 * 
 * ? v3.2  2021/10/28
 * 简化对CCS支持的printf代码
 *
 * ? v3.1  2021/10/18
 * 添加对CCS的printf支持
 *
 * ? v3.0  2021/10/15
 * 此版本支持使用 标准C库
 * 文件正式改名为与正点原子同名的
 * usart.c 和 usart.h，方便移植
 * 仅支持Keil平台开发
 *  
 * ? v2.1  2021/8/27
 * 添加支持固件库v3_21_00_05
 * 仅支持 MicroLIB 微库、Keil平台开发
 * 
 * ? v2.0  2021/8/25
 * uart_init增添了波特率传入参数，可直接配置波特率。
 * 计算UART的代码单独打包为名为
 * baudrate_calculate的c文件和h文件
 * 仅支持 MicroLIB 微库、Keil平台开发
 * 
 * ? v1.0 2021/7/17
 * 仅支持固件库v3_40_01_02
 * 配置了SMCLK 48MHz 波特率 115200的初始化代码，
 * 对接标准输入输出库，使其能使用printf、scanf函数
 * 仅支持 MicroLIB 微库、Keil平台开发
 * 
 ****************************************************/

#pragma region stdio Adaption
extern "C" {
#include "usart.h"
#include "baudrate_calculate.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#if USE_CPP && defined(__cplusplus)
}
using namespace std;
#endif

#ifdef __TI_COMPILER_VERSION__
//CCS平台
uint8_t  USART0_TX_BUF[USART0_MAX_SEND_LEN];             //发送缓冲,最大USART3_MAX_SEND_LEN字节
int printf(const char *str, ...)
{
    uint16_t i,j;
    va_list ap;
    va_start(ap,str);
    vsprintf((char*)USART0_TX_BUF,str,ap);
    va_end(ap);
    i=strlen((const char*)USART0_TX_BUF);       //此次发送数据的长度
    for(j=0;j<i;j++)                            //循环发送数据
    {
      //while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕
        UART_transmitData(EUSCI_A0_BASE, USART0_TX_BUF[j]);
    }
    return 0;
}
/*****************   函数说明   *****************
 *
 * 函数：int printf(const char *str, ...);
 * 源码来自@正点原子
 * 稍作改动适配CCS工程，在此也表感谢正点原子。
 *
 *****************   说明结束   *****************/

#else
//Keil支持标准C库跟微库
//预编译
//if 1 使用标准C库 如果报错就使用微库
//if 0 使用微库 得去勾选魔术棒里的 Use MicroLIB
// NOTE: MicroLIB is not supported when C++ is used. Condition has been updated to automatically avoid this issue. -Shiki
#if 1 | USE_CPP
// #pragma import(__use_no_semihosting)
//标准库需要的支持函数
#if USE_CPP
  void _ttywrch(int ch)
  {
    if (ch == '\n')
      UART_transmitData(EUSCI_A0_BASE, (uint8_t *)"\r\n" & 0xFF);
    else
      UART_transmitData(EUSCI_A0_BASE, (uint8_t *)&ch & 0xFF);
  }
  namespace std {
#endif
struct __FILE
{
  int handle;
};
FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
  x = x;
}
#else
int fgetc(FILE *f)
{
  while (EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG !=
         UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG))
    ;
  return UART_receiveData(EUSCI_A0_BASE);
}
#endif
int fputc(int ch, FILE *f)
{
  UART_transmitData(EUSCI_A0_BASE, ch & 0xFF);
  return ch;
}
#if USE_CPP
  }
#endif
/*****************   函数说明   *****************
 *
 * 以上两条对接标准输入输出库的函数:
 * int fputc(int ch, FILE *f);
 * int fgetc(FILE *f);
 * 源码为BiliBili平台UP主 “CloudBoyStudio” 编写
 * 本人RNA，不是作者
 * 在此也表感谢
 *
 *****************   说明结束   *****************/
#endif

#pragma endregion // stdio Adaption

#if USE_CPP

class usart
{
private:
  /* data */
public:
  usart(/* args */);
  ~usart();
};

usart::usart(/* args */)
{
}

usart::~usart()
{
}


#else
/**
 * @brief Initialize specified EUSCI_A module for UART operation.
 * 
 * @param UCA_Module  EUSCI_A module. Valid parameters: - EUSCI_A0_BASE - EUSCI_A1_BASE - EUSCI_A2_BASE - EUSCI_A3_BASE
 * @param baudRate    Baud rate for UART communication.
 * 
 * @note              EUSCI_A0 can be connected using the on-board debugger, A1 and A2 is connected to CH340 USB-C port on the Connections Extension board. Note that the RX of A1
 *                    has to be manually connected to the TTL pin 3/4 since it's not available using boosterpack standard connection.
 */
void uart_init(uint32_t UCA_Module, uint32_t baudRate)
{
#ifdef EUSCI_A_UART_7_BIT_LEN
  //固件库v3_40_01_02
  //默认SMCLK 48MHz 比特率 115200
  const eUSCI_UART_ConfigV1 uartConfig =
      {
          EUSCI_A_UART_CLOCKSOURCE_SMCLK,                // SMCLK Clock Source
          26,                                            // BRDIV = 26
          0,                                             // UCxBRF = 0
          111,                                           // UCxBRS = 111
          EUSCI_A_UART_NO_PARITY,                        // No Parity
          EUSCI_A_UART_LSB_FIRST,                        // MSB First
          EUSCI_A_UART_ONE_STOP_BIT,                     // One stop bit
          EUSCI_A_UART_MODE,                             // UART mode
          EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION, // Oversampling
          EUSCI_A_UART_8_BIT_LEN                         // 8 bit data length
      };
  eusci_calcBaudDividers((eUSCI_UART_ConfigV1 *)&uartConfig, baudRate); //配置波特率
#else
  //固件库v3_21_00_05
  //默认SMCLK 48MHz 比特率 115200
  const eUSCI_UART_Config uartConfig =
      {
          EUSCI_A_UART_CLOCKSOURCE_SMCLK,                // SMCLK Clock Source
          26,                                            // BRDIV = 26
          0,                                             // UCxBRF = 0
          111,                                           // UCxBRS = 111
          EUSCI_A_UART_NO_PARITY,                        // No Parity
          EUSCI_A_UART_LSB_FIRST,                        // MSB First
          EUSCI_A_UART_ONE_STOP_BIT,                     // One stop bit
          EUSCI_A_UART_MODE,                             // UART mode
          EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION, // Oversampling
      };
  eusci_calcBaudDividers((eUSCI_UART_Config *)&uartConfig, baudRate); //配置波特率
#endif

  MAP_UART_initModule(UCA_Module, &uartConfig);
  MAP_UART_enableModule(UCA_Module);
}

/**
 * @brief Initialize EUSCI_A0 for UART operation and configure receive interrupt.
 * 
 * @param UCA_Module  EUSCI_A module. Valid parameters: - EUSCI_A0_BASE - EUSCI_A1_BASE - EUSCI_A2_BASE - EUSCI_A3_BASE
 * @param baudRate    Baud rate for the UART module.
 * 
 * @note An ISR override [ void EUSCIA0_IRQHandler() ] is required to handle the interrupt.
 */
void uart_init_IT(uint32_t UCA_Module, uint32_t baudRate)
{
#ifdef EUSCI_A_UART_7_BIT_LEN
  // 固件库v3_40_01_02
  // 默认SMCLK 48MHz 比特率 115200
  const eUSCI_UART_ConfigV1 uartConfig =
      {
          EUSCI_A_UART_CLOCKSOURCE_SMCLK,                // SMCLK Clock Source
          26,                                            // BRDIV = 26
          0,                                             // UCxBRF = 0
          111,                                           // UCxBRS = 111
          EUSCI_A_UART_NO_PARITY,                        // No Parity
          EUSCI_A_UART_LSB_FIRST,                        // MSB First
          EUSCI_A_UART_ONE_STOP_BIT,                     // One stop bit
          EUSCI_A_UART_MODE,                             // UART mode
          EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION, // Oversampling
          EUSCI_A_UART_8_BIT_LEN                         // 8 bit data length
      };
  eusci_calcBaudDividers((eUSCI_UART_ConfigV1 *)&uartConfig, baudRate); // 配置波特率
#else
  // 固件库v3_21_00_05
  // 默认SMCLK 48MHz 比特率 115200
  const eUSCI_UART_Config uartConfig =
      {
          EUSCI_A_UART_CLOCKSOURCE_SMCLK,                // SMCLK Clock Source
          26,                                            // BRDIV = 26
          0,                                             // UCxBRF = 0
          111,                                           // UCxBRS = 111
          EUSCI_A_UART_NO_PARITY,                        // No Parity
          EUSCI_A_UART_LSB_FIRST,                        // MSB First
          EUSCI_A_UART_ONE_STOP_BIT,                     // One stop bit
          EUSCI_A_UART_MODE,                             // UART mode
          EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION, // Oversampling
      };
  eusci_calcBaudDividers((eUSCI_UART_Config *)&uartConfig, baudRate); // 配置波特率
#endif

  MAP_UART_initModule(UCA_Module, &uartConfig);
  MAP_UART_enableModule(UCA_Module);

  UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
  Interrupt_enableInterrupt(INT_EUSCIA0);
  Interrupt_enableMaster();
}

// Section: UART Receiver Interrupt Handler

UART_RxCtrl rc_a0;
UART_RxCtrl rc_a1;
UART_RxCtrl rc_a2;
UART_RxCtrl rc_a3;

/**
 * @brief Lets UART A0 keep receiving data, until it receives a newline, and immediately calls the specified callback function
 * 
 * @param rc          Pointer to the UART_RxCtrl. Use [ extern UART_RxCtrl rc_ax; ] ,where x is the UART module number, to access the UART_RxCtrl.
 * @param callback    Function to be called when reception ends
 * @param autoExpand  Whether to automatically expand the buffer. The buffer size defaults to 8 when set to true and doubles its size when full. 
 * When set to false, the buffer size is limited by the macro definition in usart.h
 * 
 * @warning The function may drop all remaining data after the buffer overflows. If autoexpand is set to false, NEVER let it overflow.
 * 
 * @note Call RxReload() instead of this function to continue receiving.
 * 
 * @note Calling rxHandler() is required in the ISR override to make this function work.
 */
void uart_RxLine(UART_RxCtrl *rc, void (*callback)(uint8_t *data, uint8_t len), bool autoExpand)
{
  rc->rxCallback = callback;
  rc->rxCnt = 0;
  rc->rxCplt = 0;
  rc->bufSize = autoExpand << 3;
  if(autoExpand){
    rc->rxBuf = (uint8_t *)malloc(8);
  }
  else{
    rc->rxBuf = (uint8_t *)malloc(UART_RX_BUF_SIZE);
  }
}
/**
 * @brief Resets the software UART receiver, allowing it to continue receiving data.
 * 
 * @param rc  Pointer to the UART_RxCtrl. Use [ extern UART_RxCtrl rc_ax; ] ,where x is the UART module number, to access the UART_RxCtrl.
 */
void rxReset(UART_RxCtrl *rc){
  rc->rxCnt = 0;
  rc->rxCplt = 0;
  rc->bufSize = rc->bufSize ? 8 : 0;
  free(rc->rxBuf);
  if (rc->bufSize)
  {
    rc->rxBuf = (uint8_t *)malloc(8);
  }
  else
  {
    rc->rxBuf = (uint8_t *)malloc(UART_RX_BUF_SIZE);
  }
  rc->rxBuf = (uint8_t *)malloc(rc->bufSize);
}

/**
 * @brief Handles incoming UART data if software handler is used. Call this function in the ISR override.
 * 
 * @param rc    Pointer to the UART_RxCtrl. Use [ extern UART_RxCtrl rc_ax; ] ,where x is the UART module number, to access the UART_RxCtrl.
 * @param data  Data received from the UART module.
 */
inline void rxHandler(UART_RxCtrl *rc, uint8_t data){
  if(rc->rxCplt){
    printf("WARN> UART Rx receiving data, but reception is not ready. Data has been dropped.\n");
    return;
  }
  if(data == 0 || data == '\n'){
    // Rx complete
    rc->rxCplt = 1;
    rc->rxBuf[rc->rxCnt++] = 0;
    rc->rxCallback(rc->rxBuf, --(rc->rxCnt));
    return;
  }
  rc->rxBuf[rc->rxCnt++] = data;
  // rxCnt = received size from here on
  if(rc->bufSize && (rc->rxCnt == rc->bufSize)){
    // Auto reallocation
    rc->bufSize <<= 1;
    uint8_t *tmp = rc->rxBuf;
    if ((rc->rxBuf = (uint8_t *)realloc(rc->rxBuf, rc->bufSize)) == NULL)
    {
      // Reallocation unsuccessful
      printf("ERR> UART Rx buffer reallocation failed. RxCtrl has been reset.\n");
      free(tmp);
      rxReset(rc);
      return;
    }
    if(rc->rxBuf != tmp){
      // Move to reallocated buffer
      memmove(rc->rxBuf, tmp, rc->rxCnt);
      free(tmp);
    }
  }
  else if(!rc->bufSize && (rc->rxCnt + 1 == UART_RX_BUF_SIZE)){
    // Buffer full, invoke callback & reset
    rc->rxCplt = 1;
    rc->rxBuf[rc->rxCnt++] = 0;
    rc->rxCallback(rc->rxBuf, --(rc->rxCnt));
    // rxReset(rc);
  }
}

/**
 * @brief Reloads the UART A0 and continues receiving data, until a newline is received and callback function is called again.
 * 
 * @param rc Pointer to the UART_RxCtrl. Use [ extern UART_RxCtrl rc_ax; ] ,where x is the UART module number, to access the UART_RxCtrl.
 */
inline void uart_RxReload(UART_RxCtrl *rc)
{
  rxReset(rc);
  rc->rxCplt = 0;
}

/**
 * @brief         Start transmission of data from a specified UART peripheral.
 *
 * @param module  instance of the eUSCI A (UART) module.
 *                Valid parameters include: - EUSCI_A0_BASE - EUSCI_A1_BASE - EUSCI_A2_BASE - EUSCI_A3_BASE
 * @param fmt     Format string
 * @param ...     Variable argument list
 * 
 * @note          The EUSCI_A0 is used exclusively for debugging, and can transmit data by using printf() function.
 */
void tx(uint32_t module, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  char *buf = (char *)malloc(64);
  int r = vsnprintf(buf, 64, fmt, ap);
  if(r == -1) {
    printf("ERR> UART Tx buffer write failed. Aborting transmission.\n");
    return;
  }
  else if(r > 63) {
    printf("ERR> UART Tx buffer overflow, data will not be transmitted.\n");
    printf("To fix this issue, print the data in smaller chunks.\n");
    return;
  }
  while(*buf) {
    UART_transmitData(module, *buf++);
  }
}

#endif // USE_CPP

#if !USE_CPP || !defined(__cplusplus)
}
#endif
