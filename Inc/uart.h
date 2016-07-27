/*
 * uart.h
 *
 *  Created on: 27. jul. 2016
 *      Author: vojis
 */

#ifndef UART_H_
#define UART_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern void Error_Handler(void);

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

#endif /* UART_H_ */
