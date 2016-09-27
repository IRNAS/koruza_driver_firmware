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
//#include "AS4047D.h"
//#include "encoder.h"
#include "stepper.h"

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

/* Uncomment to get debug messages in the UART2 terminal about received message command*/
//#define DEBUG_RECEIVE_MSG_MODE

/* Uncomment to get debug messages in the UART2 terminal about GET_STATUS command*/
//#define DEBUG_STATUS_MODE

/* Uncomment to get debug messages in the UART2 terminal about MOTOR_MOVE command*/
//#define DEBUG_MOTOR_MOVE_MODE

/* Uncomment to get debug messages in the UART2 terminal about encoder positions */
#define DEBUG_ENCODER_POSITION_MODE

/* Uncoment to get generated TLV message at the beginning */
//#define DEBUG_MODE_MSG_GENERATOR

typedef enum states{
	IDLE = 1,
	MESSAGE_PARSE = 2,
	ACTICVE_STATE = 3,
	ERROR_STATE = 4,
	END_STATE = 5
}driver_state_t;


void SystemClock_Config(void);
void Error_Handler(void);


#endif /* MAIN_H_ */
