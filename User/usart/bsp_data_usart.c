/**
  ******************************************************************************
  * @file    bsp_data_usart.c
  * @author  fire
  * @version V1.0
  * @date    2025-xx-xx
  * @brief   ʹ�ô���3��������λ��������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 F429 ������  F407
  *
  ******************************************************************************
  */ 
#include "bsp_data_usart.h"

UART_HandleTypeDef Uart3Handle;

void DATA_USART_Config(void)
{ 
  Uart3Handle.Instance=DATA_USART;
    
  Uart3Handle.Init.BaudRate     = DATA_USART_BAUDRATE;
  Uart3Handle.Init.WordLength   = UART_WORDLENGTH_8B;
  Uart3Handle.Init.StopBits     = UART_STOPBITS_1;
  Uart3Handle.Init.Parity       = UART_PARITY_NONE;
  Uart3Handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  Uart3Handle.Init.Mode         = UART_MODE_TX_RX;
  HAL_UART_Init(&Uart3Handle);
    
     /*ʹ�ܴ��ڽ��ն� */
  __HAL_UART_ENABLE_IT(&Uart3Handle,UART_IT_RXNE); 
    
}



