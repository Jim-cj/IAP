#ifndef DATA_USART_H
#define DATA_USART_H

#include "stm32f4xx.h"
#include <stdio.h>

//串口波特率
#define DATA_USART_BAUDRATE                    115200

//引脚定义
/*******************************************************/
#define DATA_USART                             USART3
#define DATA_USART_CLK_ENABLE()                __USART3_CLK_ENABLE();

//#define RCC_PERIPHCLK_UARTx                     RCC_PERIPHCLK_USART3
//#define RCC_UARTxCLKSOURCE_SYSCLK               RCC_USART3CLKSOURCE_SYSCLK

#define DATA_USART_RX_GPIO_PORT                GPIOB
#define DATA_USART_RX_GPIO_CLK_ENABLE()        __GPIOB_CLK_ENABLE()
#define DATA_USART_RX_PIN                      GPIO_PIN_11
#define DATA_USART_RX_AF                       GPIO_AF7_USART3

#define DATA_USART_TX_GPIO_PORT                GPIOB
#define DATA_USART_TX_GPIO_CLK_ENABLE()        __GPIOB_CLK_ENABLE()
#define DATA_USART_TX_PIN                      GPIO_PIN_10
#define DATA_USART_TX_AF                       GPIO_AF7_USART3

#define DATA_USART_IRQHandler                  USART3_IRQHandler
#define DATA_USART_IRQ                 		    USART3_IRQn
/************************************************************/

void Usart_SendString(uint8_t *str);
void DATA_USART_Config(void);
//int fputc(int ch, FILE *f);
extern UART_HandleTypeDef Uart3Handle;

#endif
