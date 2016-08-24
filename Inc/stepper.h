/*
 * stepper.h
 *
 *  Created on: 26. jul. 2016
 *      Author: vojis
 */
#include "AccelStepper.h"
#include "message.h"

#ifndef STEPPER_H_
#define STEPPER_H_

extern tlv_motor_position_t current_motor_position;

#define MOTOR_PIN_X_1 GPIO_PIN_13
#define MOTOR_PIN_X_2 GPIO_PIN_14
#define MOTOR_PIN_X_3 GPIO_PIN_15
#define MOTOR_PIN_X_4 GPIO_PIN_9

#define MOTOR_PORT_X_1 GPIOC
#define MOTOR_PORT_X_2 GPIOC
#define MOTOR_PORT_X_3 GPIOC
#define MOTOR_PORT_X_4 GPIOC


#define MOTOR_PIN_Y_1 GPIO_PIN_2
#define MOTOR_PIN_Y_2 GPIO_PIN_3
#define MOTOR_PIN_Y_3 GPIO_PIN_5
#define MOTOR_PIN_Y_4 GPIO_PIN_11

#define MOTOR_PORT_Y_1 GPIOC
#define MOTOR_PORT_Y_2 GPIOC
#define MOTOR_PORT_Y_3 GPIOC
#define MOTOR_PORT_Y_4 GPIOC


#define MOTOR_PIN_Z_1 GPIO_PIN_1
#define MOTOR_PIN_Z_2 GPIO_PIN_2
#define MOTOR_PIN_Z_3 GPIO_PIN_0
#define MOTOR_PIN_Z_4 GPIO_PIN_4

#define MOTOR_PORT_Z_1 GPIOB
#define MOTOR_PORT_Z_2 GPIOB
#define MOTOR_PORT_Z_3 GPIOA
#define MOTOR_PORT_Z_4 GPIOA


/**
 * Initializes Koruza driver steppers.
 * This function should be called when program is in initialization
 *
 * @param Stepper X struct address
 * @param Stepper Y struct address
 * @param Stepper Z struct address
 * @return none
 */
void Init_koruza_motors(Stepper_t *stepper_x, Stepper_t *stepper_y, Stepper_t *stepper_z);

/**
 * Calculates how much steps motors need to be moved
 *
 * @param New motor position struct
 * @param Current motor position struct
 * @return none
 */
tlv_motor_position_t Claculate_motors_move_steps(tlv_motor_position_t *new_motor_position, tlv_motor_position_t *current_motor_position);

void run_motors(Stepper_t *stepper_x, Stepper_t *stepper_y, Stepper_t *stepper_z);

uint8_t run_motor(Stepper_t *stepper, int32_t *location, int min_pin, int max_pin);

void Set_motor_coordinate(Stepper_t *stepper, long coordinate);

void Set_motors_coordinates(Stepper_t *stepper_x, int coordinate_x, Stepper_t *stepper_y, int coordinate_y, Stepper_t *stepper_z, int coordinate_z);

void Set_home_coordinates(Stepper_t *stepper_x, Stepper_t *stepper_y, Stepper_t *stepper_z);

#endif /* STEPPER_H_ */
