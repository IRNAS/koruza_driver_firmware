/*
 * main.h
 *
 *  Created on: 22. jul. 2016
 *      Author: vojis
 */

#ifndef MAIN_H_
#define MAIN_H_

#define RXBUFFERSIZE 100
#define True 1
#define False 0

/* Uncomment to get debug messages in the UART2 terminal*/
#define DEBUG_MODE

void SystemClock_Config(void);
void Error_Handler(void);


void Init_motors(Stepper_t *stepper_x, Stepper_t *stepper_y, Stepper_t *stepper_z);
float fast_sqrt(float number);

#endif /* MAIN_H_ */
