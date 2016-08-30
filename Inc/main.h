/*
 * main.h
 *
 *  Created on: 22. jul. 2016
 *      Author: vojis
 */

#include "message.h"
#include "bootloader.h"
#include "spi.h"
#include "tim.h"
#include "AS4047D.h"
#include "encoder.h"

#ifndef MAIN_H_
#define MAIN_H_

#define RXBUFFERSIZE 100
#define True 1
#define False 0

/* OVERCURRENT_LIMIT is the value which sets the limit for over current protection
 * If current value is over this limit MCU goes to overcurrent protection state */
#define OVERCURRENT_LIMIT 5000

/* Uncomment to get debug messages in the UART2 terminal*/
#define DEBUG_MODE

enum states{
	IDLE,
	MESSAGE_PARSE,
	ACTICVE_STATE,
	ERROR_STATE,
	END_STATE
};


void SystemClock_Config(void);
void Error_Handler(void);


#endif /* MAIN_H_ */
