/*
 * stepper.c
 *
 *  Created on: 26. jul. 2016
 *      Author: vojis
 */

#include "stepper.h"
#include "stm32f4xx_hal.h"
#include "AccelStepper.h"
#include <stdio.h>

void Init_koruza_motors(Stepper_t *stepper_x, Stepper_t *stepper_y, Stepper_t *stepper_z){

	/*## Initialize X axis stepper. ###*/
	InitStepper(stepper_x, HALF4WIRE, MOTOR_PIN_X_1, MOTOR_PORT_X_1, MOTOR_PIN_X_2, MOTOR_PORT_X_2, MOTOR_PIN_X_3, MOTOR_PORT_X_3, MOTOR_PIN_X_4, MOTOR_PORT_X_4, 1);
	setMaxSpeed(stepper_x, 500);
	setSpeed(stepper_x, 500);
	setAcceleration(stepper_x, 500);
	moveTo(stepper_x, 0);
	enableOutputs(stepper_x);

	/*## Initialize Y axis stepper. ###*/
	InitStepper(stepper_y, HALF4WIRE, MOTOR_PIN_Y_1, MOTOR_PORT_Y_1, MOTOR_PIN_Y_2, MOTOR_PORT_Y_2, MOTOR_PIN_Y_3, MOTOR_PORT_Y_3, MOTOR_PIN_Y_4, MOTOR_PORT_Y_4, 1);
	setMaxSpeed(stepper_y, 500);
	setSpeed(stepper_y, 500);
	setAcceleration(stepper_y, 500);
	moveTo(stepper_y, 0);
	enableOutputs(stepper_y);

	/*## Initialize Z axis stepper. ###*/
	InitStepper(stepper_z, HALF4WIRE, MOTOR_PIN_Z_1, MOTOR_PORT_Z_1, MOTOR_PIN_Z_2, MOTOR_PORT_Z_2, MOTOR_PIN_Z_3, MOTOR_PORT_Z_3, MOTOR_PIN_Z_4, MOTOR_PORT_Z_4, 1);
	setMaxSpeed(stepper_z, 500);
	setSpeed(stepper_z, 500);
	setAcceleration(stepper_z, 500);
	moveTo(stepper_z, 0);
	enableOutputs(stepper_z);

}

tlv_motor_position_t Claculate_motors_move_steps(tlv_motor_position_t *new_motor_position, tlv_motor_position_t *current_motor_position){
	tlv_motor_position_t move_steppers_for;

	if(new_motor_position->x != current_motor_position->x){
		move_steppers_for.x = new_motor_position->x - current_motor_position->x;
	}else{
		move_steppers_for.x = 0;
	}

	if(new_motor_position->y != current_motor_position->y){
		move_steppers_for.y = new_motor_position->y - current_motor_position->y;
	}else{
		move_steppers_for.y = 0;
	}

	if(new_motor_position->z != current_motor_position->z){
		move_steppers_for.z = new_motor_position->z - current_motor_position->z;
	}else{
		move_steppers_for.z = 0;
	}

	return move_steppers_for;
}
