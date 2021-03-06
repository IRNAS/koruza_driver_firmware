/*
 * stepper.c
 *
 *  Created on: 26. jul. 2016
 *      Author: vojis
 */

#include "stepper.h"
//#include "stm32f4xx_hal.h"
//#include "AccelStepper.h"
//#include "main.h"
#include <stdio.h>
#include "stdlib.h"

tlv_motor_position_t current_motor_position;
/* Stepper motors struts */
//Stepper_t stepper_motor_x;
//Stepper_t stepper_motor_y;
//Stepper_t stepper_motor_z;

koruza_steppers_t koruza_steppers;

int home_x_flag = 0;
int home_y_flag = 0;
int center_reached = 0;

void koruza_motors_init(koruza_steppers_t *steppers, stepper_connected_t stepper_con_x, stepper_connected_t stepper_con_y, stepper_connected_t stepper_con_z){

	koruza_steppers.stepper_x.stepper_connected = stepper_con_x;
	koruza_steppers.stepper_y.stepper_connected = stepper_con_y;
	koruza_steppers.stepper_z.stepper_connected = stepper_con_z;

	//steppers->stepper_x.stepper._direction = DIRECTION_CCW;
	//steppers->stepper_y.stepper._direction = DIRECTION_CCW;

	koruza_steppers.mode = STEPPERS_IDLE_MODE;

	/*## Initialize X axis stepper. ###*/
	if(steppers->stepper_x.stepper_connected == STEPPER_CONNECTED){
		steppers->stepper_x.s_mode = STEPPER_IDLE;
		InitStepper(&steppers->stepper_x.stepper, HALF4WIRE, MOTOR_PIN_X_4, MOTOR_PORT_X_4, MOTOR_PIN_X_3, MOTOR_PORT_X_3, MOTOR_PIN_X_2, MOTOR_PORT_X_2, MOTOR_PIN_X_1, MOTOR_PORT_X_1, 1);
		setMaxSpeed(&steppers->stepper_x.stepper, 500);
		setSpeed(&steppers->stepper_x.stepper, 500);
		setAcceleration(&steppers->stepper_x.stepper, 500);
		moveTo(&steppers->stepper_x.stepper, 1);
		enableOutputs(&steppers->stepper_x.stepper);
	}

	/*## Initialize Y axis stepper. ###*/
	if(steppers->stepper_y.stepper_connected == STEPPER_CONNECTED){
		steppers->stepper_y.s_mode = STEPPER_IDLE;
		InitStepper(&steppers->stepper_y.stepper, HALF4WIRE, MOTOR_PIN_Y_4, MOTOR_PORT_Y_4, MOTOR_PIN_Y_3, MOTOR_PORT_Y_3, MOTOR_PIN_Y_2, MOTOR_PORT_Y_2, MOTOR_PIN_Y_1, MOTOR_PORT_Y_1, 1);
		setMaxSpeed(&steppers->stepper_y.stepper, 500);
		setSpeed(&steppers->stepper_y.stepper, 500);
		setAcceleration(&steppers->stepper_y.stepper, 500);
		moveTo(&steppers->stepper_y.stepper, 1);
		enableOutputs(&steppers->stepper_y.stepper);
	}
	/*## Initialize Z axis stepper. ###*/
	if(steppers->stepper_z.stepper_connected == STEPPER_CONNECTED){
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

void run_motors(koruza_steppers_t *steppers, koruza_encoders_t *encoders){

	/* Koruza stepper motors moving to new sent coordinates */
	if(steppers->mode == STEPPERS_IDLE_MODE){
		/* Motor X*/
		if((steppers->stepper_x.s_mode == STEPPER_IDLE) || (steppers->stepper_x.s_mode == STEPPER_MOVING)){
			run_motor(&steppers->stepper_x, &current_motor_position.x, &encoders->encoder_x);
		}
		else if((steppers->stepper_x.s_mode == STEPPER_MAXIMUM_REACHED) || (steppers->stepper_x.s_mode == STEPPER_MINIMUM_REACHED)){
			if(steppers->stepper_x.s_mode == STEPPER_MAXIMUM_REACHED){
				set_motor_coordinate(&steppers->stepper_x.stepper, currentPosition(&steppers->stepper_x.stepper) - 1137);
			}
			else if(steppers->stepper_x.s_mode == STEPPER_MINIMUM_REACHED){
				set_motor_coordinate(&steppers->stepper_x.stepper, currentPosition(&steppers->stepper_x.stepper) + 1137);
			}
			current_motor_position.x = (int32_t)steppers->stepper_x.stepper._currentPos;
			steppers->stepper_x.s_mode = STEPPER_IDLE;
		}
		/* STEPPER_ERROR*/
		else{
			//steppers->stepper_x.mode = run_motor(&steppers->stepper_x.stepper, &current_motor_position.x, &encoders->encoder_x);
		}

		/* Motor Y*/
		if((steppers->stepper_y.s_mode == STEPPER_IDLE) || (steppers->stepper_y.s_mode == STEPPER_MOVING)){
			run_motor(&steppers->stepper_y, &current_motor_position.y, &encoders->encoder_y);
		}
		else if((steppers->stepper_y.s_mode == STEPPER_MAXIMUM_REACHED) || (steppers->stepper_y.s_mode == STEPPER_MINIMUM_REACHED)){
			if(steppers->stepper_y.s_mode == STEPPER_MAXIMUM_REACHED){
				set_motor_coordinate(&steppers->stepper_y.stepper, currentPosition(&steppers->stepper_y.stepper) - 1137);
			}
			else if(steppers->stepper_y.s_mode == STEPPER_MINIMUM_REACHED){
				set_motor_coordinate(&steppers->stepper_y.stepper, currentPosition(&steppers->stepper_y.stepper) + 1137);
			}
			current_motor_position.y = (int32_t)steppers->stepper_y.stepper._currentPos;
			steppers->stepper_y.s_mode = STEPPER_IDLE;
		}
		/* STEPPER_ERROR*/
		else{
			//steppers->stepper_y.mode = run_motor(&steppers->stepper_y.stepper, &current_motor_position.y, &encoders->encoder_y);
		}
		//steppers->stepper_z.mode = run_motor(&steppers->stepper_z.stepper, &current_motor_position.z, 1, 2);
	}
	/* STEPPERS_HOMMING_MODE Koruza stepper motors doing the homing routin */
	else{
		run_motor(&steppers->stepper_x, &current_motor_position.x, &encoders->encoder_x);
		run_motor(&steppers->stepper_y, &current_motor_position.y, &encoders->encoder_y);

		/* Whait for encoder x to reach end */
		if((steppers->stepper_x.s_mode == STEPPER_MAXIMUM_REACHED) || (steppers->stepper_x.s_mode == STEPPER_MINIMUM_REACHED) || (steppers->stepper_x.stepper._currentPos < -100000)){
			if(encoders->encoder_x.encoder_connected == CONNECTED){
				if(steppers->stepper_x.s_mode == STEPPER_MAXIMUM_REACHED){
					set_motor_coordinate(&steppers->stepper_x.stepper, currentPosition(&steppers->stepper_x.stepper) - 1137);
				}
				else if(steppers->stepper_x.s_mode == STEPPER_MINIMUM_REACHED){
					set_motor_coordinate(&steppers->stepper_x.stepper, currentPosition(&steppers->stepper_x.stepper) + 1137);
				}
			}
			else{
				set_motor_coordinate(&steppers->stepper_x.stepper, steppers->stepper_x.stepper._currentPos);
			}
			current_motor_position.x = (int32_t)steppers->stepper_x.stepper._currentPos;
			steppers->stepper_x.s_mode = STEPPER_IDLE;
			home_x_flag = 1;
		}

		/* Whait for encoder y to reach end */
		if((steppers->stepper_y.s_mode == STEPPER_MAXIMUM_REACHED) || (steppers->stepper_y.s_mode == STEPPER_MINIMUM_REACHED) || (steppers->stepper_y.stepper._currentPos < -100000)){
			if(encoders->encoder_y.encoder_connected == CONNECTED){
				if(steppers->stepper_y.s_mode == STEPPER_MAXIMUM_REACHED){
					set_motor_coordinate(&steppers->stepper_y.stepper, currentPosition(&steppers->stepper_y.stepper) - 1137);
				}
				else if(steppers->stepper_y.s_mode == STEPPER_MINIMUM_REACHED){
					set_motor_coordinate(&steppers->stepper_y.stepper, currentPosition(&steppers->stepper_y.stepper) + 1137);
				}
			}
			else{
				set_motor_coordinate(&steppers->stepper_y.stepper, steppers->stepper_y.stepper._currentPos);
			}
			current_motor_position.y = (int32_t)steppers->stepper_y.stepper._currentPos;
			steppers->stepper_y.s_mode = STEPPER_IDLE;
			home_y_flag = 1;
		}

		/* Both Koruza motors reached the maximum movement */
		if((home_x_flag == 1) && (home_y_flag == 1)){
			/* Set current position around -25000, based on encoders if connected*/
			if(encoders->encoder_x.encoder_connected == CONNECTED){
				set_motor_coordinate(&steppers->stepper_x.stepper, -25000 + encoders->encoder_x.encoder.true_angle * ONE_ANGLE_STEPPS);
			}
			else{
				set_motor_coordinate(&steppers->stepper_x.stepper, -25000);
			}
			if(encoders->encoder_y.encoder_connected == CONNECTED){
				set_motor_coordinate(&steppers->stepper_y.stepper, -25000 + encoders->encoder_y.encoder.true_angle * ONE_ANGLE_STEPPS);
			}
			else{
				set_motor_coordinate(&steppers->stepper_y.stepper, -25000);
			}
			current_motor_position.x = (int32_t)steppers->stepper_x.stepper._currentPos;
			current_motor_position.y = (int32_t)steppers->stepper_y.stepper._currentPos;

			/* Go to zero*/
			moveTo(&steppers->stepper_x.stepper, 0);
			moveTo(&steppers->stepper_y.stepper, 0);

			/* Check magnetic field after reaching end */
			koruza_encoders_magnetic_filed_check(&koruza_error_report_ch.code, &koruza_encoders, 1);
			//printf("homing routine: end reached\n");
			//printf("homing routine: go to center\n");
			/* Set the Koruza steppers mode to IDLE*/
			home_x_flag = 0;
			home_y_flag = 0;
			steppers->mode = STEPPERS_IDLE_MODE;
		}

	}
}

void run_motor(koruza_stepper_t *stepper, int32_t *location, koruza_encoder_t *encoder){
	// this function runs the stepper motor
	// returns status of limits reached and motion stopped
	// 1 - idle
	// 2 - moving
	// 3 - minimum reached
	// 4 - maximum reached
	// 5 - error

	if(encoder->end == ENCODER_END){
		/* Negative direction end reached */
		if(targetPosition(&stepper->stepper) < currentPosition(&stepper->stepper)){
			stop(&stepper->stepper);
			stepper->s_mode = STEPPER_MINIMUM_REACHED;
		}
		/* Positive direction end reached */
		if(targetPosition(&stepper->stepper) > currentPosition(&stepper->stepper)){
			stop(&stepper->stepper);
			stepper->s_mode = STEPPER_MAXIMUM_REACHED;
		}
		//moveTo(stepper, currentPosition(stepper));
		//*location = (int32_t)stepper->_currentPos;
	}
	/* encoder.end is in ENCODER_RUN mode */
	else{
		// motor movement
		// motor pins are enabled only while moving to conserve power
		if(currentPosition(&stepper->stepper) != targetPosition(&stepper->stepper)){
			enableOutputs(&stepper->stepper);
			run(&stepper->stepper);

			// update the current position
			*location = (int32_t)currentPosition(&stepper->stepper);

			// return moving status
			stepper->s_mode=STEPPER_MOVING;

			//reset timeout write for storing position
			//timeout_write_flash=millis()+30000;
			//write_enable=HIGH;
		}// idle
		else{
			stop(&stepper->stepper);
			disableOutputs(&stepper->stepper);
			stepper->s_mode=STEPPER_IDLE;
		}
	}
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

void koruza_set_false_zero(koruza_encoders_t *encoders, koruza_steppers_t *steppers, tlv_motor_position_t *currnet_position){
	if(encoders->encoder_x.encoder_connected == CONNECTED){
		set_motor_coordinate(&steppers->stepper_x.stepper, (long)(encoders->encoder_x.encoder.true_angle * ONE_ANGLE_STEPPS));
		currnet_position->x = (int32_t)steppers->stepper_x.stepper._currentPos;
	}else{
		set_motor_coordinate(&steppers->stepper_x.stepper, 0);
		currnet_position->x = 0;
	}
	if(encoders->encoder_y.encoder_connected == CONNECTED){
		set_motor_coordinate(&steppers->stepper_y.stepper, (long)(encoders->encoder_y.encoder.true_angle * ONE_ANGLE_STEPPS));
		currnet_position->y = (int32_t)steppers->stepper_y.stepper._currentPos;
	}else{
		set_motor_coordinate(&steppers->stepper_y.stepper, 0);
		currnet_position->y = 0;
	}
}

void koruza_encoder_stepper_error(koruza_steppers_t *steppers, koruza_encoders_t *encoders){
	if(encoders->encoder_x.encoder_connected == CONNECTED){
		if(labs((long)encoders->encoder_x.steps - steppers->stepper_x.stepper._currentPos) > ENCODER_STEPPER_MAX_ERROR){
			/*if(encoders->encoder_x.turn_cnt >= 0){
				encoders->encoder_x.end = ENCODER_END_MAX;
			}
			else{
				encoders->encoder_x.end = ENCODER_END_MIN;
			}*/
			encoders->encoder_x.end = ENCODER_END;
		}
		else{
			encoders->encoder_x.end = ENCODER_RUN;
		}
	}
	else{
		/* If the encoder is not connected it will be in the ENCODER_RUN mode, so the motors can move*/
		encoders->encoder_x.end = ENCODER_RUN;
	}

	if(encoders->encoder_y.encoder_connected == CONNECTED){
		if(labs((long)encoders->encoder_y.steps - steppers->stepper_y.stepper._currentPos) > ENCODER_STEPPER_MAX_ERROR){
			/*if(encoders->encoder_y.turn_cnt >= 0){
				encoders->encoder_y.end = ENCODER_END_MAX;
			}
			else{
				encoders->encoder_y.end = ENCODER_END_MIN;
			}*/
			encoders->encoder_y.end = ENCODER_END;
		}
		else{
			encoders->encoder_y.end = ENCODER_RUN;
		}
	}
	else{
		/* If the encoder is not connected it will be in the ENCODER_RUN mode, so the motors can move*/
		encoders->encoder_y.end = ENCODER_RUN;
	}

}
/* function returning the max between two numbers */
static double max(double num1, double num2) {
   /* local variable declaration */
   double result;

   if (num1 > num2)
      result = num1;
   else
      result = num2;

   return result;
}

/* function returning the min between two numbers */
static double min(double num1, double num2) {
   /* local variable declaration */
   double result;

   if (num1 < num2)
      result = num1;
   else
      result = num2;

   return result;
}

void koruza_steppers_encoder_error_calculation(koruza_encoders_t *encoders, koruza_steppers_t *steppers){
	double stepper_x_angle;
	long temp = koruza_steppers.stepper_x.stepper._currentPos;
	stepper_x_angle = (double)(temp % 4096) / ONE_ANGLE_STEPPS;
	//steppers->stepper_x.mode = STEPPER_MAXIMUM_REACHED;
	double stepper_y_angle = (steppers->stepper_y.stepper._currentPos % 4096) / ONE_ANGLE_STEPPS;

	double encoder_x_angle = (double)(encoders->encoder_x.encoder.true_angle);
	double encoder_y_angle = (double)(encoders->encoder_y.encoder.true_angle);

	if(encoders->encoder_x.encoder_connected == CONNECTED){
		encoders->encoder_x.diff = max(stepper_x_angle, encoder_x_angle) - min(stepper_x_angle, encoder_x_angle);
		if(encoders->encoder_x.diff > 180){
			encoders->encoder_x.diff = 360 - encoders->encoder_x.diff;
		}

		if(abs(encoders->encoder_x.diff) > DIFF_END_SW){
			encoders->encoder_x.end = ENCODER_END;
		}else{
			encoders->encoder_x.end = ENCODER_RUN;
		}
	}
	if(encoders->encoder_y.encoder_connected == CONNECTED){
		encoders->encoder_y.diff = max(stepper_y_angle, encoder_y_angle) - min(stepper_y_angle, encoder_y_angle);
		if(encoders->encoder_y.diff > 180){
			encoders->encoder_y.diff = 360 - encoders->encoder_y.diff;
		}

		if(abs(encoders->encoder_y.diff) > DIFF_END_SW){
			encoders->encoder_y.end = ENCODER_END;
		}else{
			encoders->encoder_y.end = ENCODER_RUN;
		}
	}
}

void koruza_set_stored_values(koruza_encoders_t *encoders, koruza_steppers_t *steppers, tlv_motor_position_t stored_motor_values){
	/* Set stepper motor position  and current position*/
	set_motors_coordinates(steppers, stored_motor_values.x, stored_motor_values.y, stored_motor_values.z);
	/* Calculate and set encoder position*/
	/*
	if(encoders->encoder_x.encoder_connected == CONNECTED){
		encoders->encoder_x.abs_angle = labs(stored_motor_values.x) / ONE_ANGLE_STEPPS;
		if(stored_motor_values.x >= 0){
			encoders->encoder_x.turn_cnt = (int)(stored_motor_values.x / 4096);
			encoders->encoder_x.last_angle = (float)(360 - ((int)encoders->encoder_x.abs_angle % 360));
		}
		else{
			encoders->encoder_x.turn_cnt = (int)(stored_motor_values.x / 4096) - 1;
			encoders->encoder_x.last_angle = (float)((int)encoders->encoder_x.abs_angle % 360);
		}
		encoders->encoder_x.new_angle = encoders->encoder_x.last_angle;
	}

	if(encoders->encoder_y.encoder_connected == CONNECTED){
		encoders->encoder_y.abs_angle = labs(stored_motor_values.y) / ONE_ANGLE_STEPPS;
		if(stored_motor_values.y >= 0){
			encoders->encoder_y.turn_cnt = (int)(stored_motor_values.y / 4096);
			encoders->encoder_y.last_angle = (float)(360 - ((int)encoders->encoder_y.abs_angle % 360));
		}
		else{
			encoders->encoder_y.turn_cnt = (int)(stored_motor_values.y / 4096) - 1;
			encoders->encoder_y.last_angle = (float)((int)encoders->encoder_y.abs_angle % 360);
		}
		encoders->encoder_y.new_angle = encoders->encoder_y.last_angle;
	}*/
}

