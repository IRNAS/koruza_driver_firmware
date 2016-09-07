/*
 * stepper.c
 *
 *  Created on: 26. jul. 2016
 *      Author: vojis
 */

#include "stepper.h"
#include "stm32f4xx_hal.h"
#include "AccelStepper.h"
#include "main.h"
#include <stdio.h>

tlv_motor_position_t current_motor_position;
/* Stepper motors struts */
//Stepper_t stepper_motor_x;
//Stepper_t stepper_motor_y;
//Stepper_t stepper_motor_z;

koruza_steppers_t koruza_steppers;

void koruza_motors_init(koruza_steppers_t *steppers, stepper_connected_t stepper_con_x, stepper_connected_t stepper_con_y, stepper_connected_t stepper_con_z){

	koruza_steppers.stepper_x.encoder_connected = stepper_con_x;
	koruza_steppers.stepper_y.encoder_connected = stepper_con_y;
	koruza_steppers.stepper_z.encoder_connected = stepper_con_z;

	/*## Initialize X axis stepper. ###*/
	if(steppers->stepper_x.encoder_connected == STEPPER_CONNECTED){
		InitStepper(&steppers->stepper_x.stepper, HALF4WIRE, MOTOR_PIN_X_4, MOTOR_PORT_X_4, MOTOR_PIN_X_3, MOTOR_PORT_X_3, MOTOR_PIN_X_2, MOTOR_PORT_X_2, MOTOR_PIN_X_1, MOTOR_PORT_X_1, 1);
		setMaxSpeed(&steppers->stepper_x.stepper, 500);
		setSpeed(&steppers->stepper_x.stepper, 500);
		setAcceleration(&steppers->stepper_x.stepper, 500);
		moveTo(&steppers->stepper_x.stepper, 1);
		enableOutputs(&steppers->stepper_x.stepper);
	}

	/*## Initialize Y axis stepper. ###*/
	if(steppers->stepper_y.encoder_connected == STEPPER_CONNECTED){
		InitStepper(&steppers->stepper_y.stepper, HALF4WIRE, MOTOR_PIN_Y_4, MOTOR_PORT_Y_4, MOTOR_PIN_Y_3, MOTOR_PORT_Y_3, MOTOR_PIN_Y_2, MOTOR_PORT_Y_2, MOTOR_PIN_Y_1, MOTOR_PORT_Y_1, 1);
		setMaxSpeed(&steppers->stepper_y.stepper, 500);
		setSpeed(&steppers->stepper_y.stepper, 500);
		setAcceleration(&steppers->stepper_y.stepper, 500);
		moveTo(&steppers->stepper_y.stepper, 1);
		enableOutputs(&steppers->stepper_y.stepper);
	}
	/*## Initialize Z axis stepper. ###*/
	if(steppers->stepper_z.encoder_connected == STEPPER_CONNECTED){
		InitStepper(&steppers->stepper_z.stepper, HALF4WIRE, MOTOR_PIN_Z_4, MOTOR_PORT_Z_4, MOTOR_PIN_Z_3, MOTOR_PORT_Z_3, MOTOR_PIN_Z_2, MOTOR_PORT_Z_2, MOTOR_PIN_Z_1, MOTOR_PORT_Z_1, 1);
		setMaxSpeed(&steppers->stepper_z.stepper, 500);
		setSpeed(&steppers->stepper_z.stepper, 500);
		setAcceleration(&steppers->stepper_z.stepper, 500);
		moveTo(&steppers->stepper_z.stepper, 1);
		enableOutputs(&steppers->stepper_z.stepper);
	}

	current_motor_position.x = 0;
	current_motor_position.y = 0;
	current_motor_position.z = 0;

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

void run_motors(koruza_steppers_t *steppers){
	//TODO: pogledaj da li stvarno trebaju ostali argumenti
	run_motor(&steppers->stepper_x.stepper, &current_motor_position.x, 1, 2);
	run_motor(&steppers->stepper_y.stepper, &current_motor_position.y, 1, 2);
	run_motor(&steppers->stepper_z.stepper, &current_motor_position.z, 1, 2);

}

uint8_t run_motor(Stepper_t *stepper, int32_t *location, int min_pin, int max_pin){
	// this function runs the stepper motor
	// returns status of limits reached and motion stopped
	// 0x00 - idle
	// 0x01 - moving
	// 0x1* - minimum reached
	// 0x2* - maximum reached
	// 0xff - error

	uint8_t return_data = 0x00;

	/* Negative direction end reached */
	//if(koruza_encoder_end()){

	//}

	/* Positive direction end reached */
	//if(koruza_encoder_end()){

	//}
	// motor movement
	// motor pins are enabled only while moving to conserve power
	if(currentPosition(stepper) != targetPosition(stepper)){
		enableOutputs(stepper);
		run(stepper);

		// update the current position
		*location = (int32_t)currentPosition(stepper);

		// return moving status
		return_data=(return_data|0x01);

		//reset timeout write for storing position
		//timeout_write_flash=millis()+30000;
		//write_enable=HIGH;
	}// idle
	else{
		stop(stepper);
		disableOutputs(stepper);
		return_data=(return_data|0x00);
	}
	return return_data;
}

void set_motor_coordinate(Stepper_t *stepper, long coordinate){
	setCurrentPosition(stepper, coordinate);
}

void set_motors_coordinates(koruza_steppers_t *steppers, int coordinate_x, int coordinate_y, int coordinate_z){
	set_motor_coordinate(&steppers->stepper_x.stepper, coordinate_x);
	set_motor_coordinate(&steppers->stepper_y.stepper, coordinate_y);
	set_motor_coordinate(&steppers->stepper_z.stepper, coordinate_z);

	current_motor_position.x = coordinate_x;
	current_motor_position.y = coordinate_y;
	current_motor_position.z = coordinate_z;
}

void set_home_coordinates(koruza_steppers_t *steppers){
	set_motor_coordinate(&steppers->stepper_x.stepper, 0);
	set_motor_coordinate(&steppers->stepper_y.stepper, 0);
	set_motor_coordinate(&steppers->stepper_z.stepper, 0);

	current_motor_position.x = 0;
	current_motor_position.y = 0;
	current_motor_position.z = 0;
}

void koruza_homing(koruza_steppers_t *steppers){

	//run_motors(stepper_x, stepper_y, stepper_z);
}
