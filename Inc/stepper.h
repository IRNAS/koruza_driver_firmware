/*
 * stepper.h
 *
 *  Created on: 26. jul. 2016
 *      Author: vojis
 */
#include "AccelStepper.h"
#include "message.h"
#include "encoder.h"

#ifndef STEPPER_H_
#define STEPPER_H_

extern tlv_motor_position_t current_motor_position;
//extern Stepper_t stepper_motor_x;
//extern Stepper_t stepper_motor_y;
//extern Stepper_t stepper_motor_z;

typedef enum{
	STEPPER_IDLE = 0,
	STEPPER_MOVING = 1,
	STEPPER_MINIMUM_REACHED = 2,
	STEPPER_MAXIMUM_REACHED = 3,
	STEPPER_ERROR = 4,
}koruza_stepper_mode;

typedef enum{
	STEPPER_NOT_CONNECTED = 0,
	STEPPER_CONNECTED = 1,
}stepper_connected_t;

typedef enum{
	STEPPERS_IDLE_MODE = 0,
	STEPPERS_HOMING_MODE = 1,
}koruza_steppers_mode;

typedef struct{
	Stepper_t stepper;
	stepper_connected_t stepper_connected;
	koruza_stepper_mode mode;
}koruza_stepper_t;

typedef struct{
	koruza_stepper_t stepper_x;
	koruza_stepper_t stepper_y;
	koruza_stepper_t stepper_z;
	koruza_steppers_mode mode;
}koruza_steppers_t;

extern koruza_steppers_t koruza_steppers;

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

#define HOME_X_COORDINATE 50000
#define HOME_Y_COORDINATE HOME_X_COORDINATE

//TODO: put the right values for the center coordinates of the stepper
#define STEPPER_X_CENTER 100
#define STEPPER_Y_CENTER 100
/**
 * Initializes Koruza driver steppers.
 * This function should be called when program is in initialization
 *
 * @param Stepper X struct address
 * @param Stepper Y struct address
 * @param Stepper Z struct address
 * @return none
 */
void koruza_motors_init(koruza_steppers_t *steppers, stepper_connected_t stepper_con_x, stepper_connected_t stepper_con_y, stepper_connected_t stepper_con_z);

/**
 * Calculates how much steps motors need to be moved
 *
 * @param New motor position struct
 * @param Current motor position struct
 * @return none
 */
tlv_motor_position_t Claculate_motors_move_steps(tlv_motor_position_t *new_motor_position, tlv_motor_position_t *current_motor_position);

void run_motors(koruza_steppers_t *steppers, koruza_encoders_t *encoders);

koruza_stepper_mode run_motor(Stepper_t *stepper, int32_t *location, int min_pin, int max_pin);

void set_motor_coordinate(Stepper_t *stepper, long coordinate);

void set_motors_coordinates(koruza_steppers_t *steppers, int coordinate_x, int coordinate_y, int coordinate_z);

void set_home_coordinates(koruza_steppers_t *steppers);

void koruza_homing(koruza_steppers_t *steppers);

void koruza_encoder_stepper_error(koruza_steppers_t *steppers, koruza_encoders_t *encoders);

#endif /* STEPPER_H_ */
