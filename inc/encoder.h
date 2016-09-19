/*
 * encoder.h
 *
 *  Created on: 31. avg. 2016
 *      Author: vojis
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "AS4047D.h"
#include "gpio.h"
//#include "stepper.h"
//TODO: proveri koji bi ovo bio ugao
/* Maximum angle to check when the full circle*/
#define MAX_DIF_ANGLE 200   //degrees
//#define ENCODER_END_DIF 20 //degrees
//TODO: check for the real value, 4096 is only for test homing
#define ENCODER_STEPPER_MAX_ERROR 1000//100

#define STEPS_PER_ROTATION 4096
#define ONE_ANGLE_STEPPS 11.37777777777778

#define CORRECTION_FACOTR 1.2
/* Encoder X port and pin for chip select*/
#define AS4047D_CS1_Port GPIOB
#define AS4047D_CS1_Pin GPIO_PIN_12

/* Encoder Y port and pin for chip select*/
#define AS4047D_CS2_Port GPIOC
#define AS4047D_CS2_Pin GPIO_PIN_6

typedef enum{
	ENCODER_END_MAX = 0,
	ENCODER_END_MIN = 1,
	ENCODER_END = 2,
	ENCODER_RUN = 3,
}encoder_end_t;

typedef enum{
	NOT_CONNECTED = 0,
	CONNECTED = 1,
}encoder_connected_t;

typedef struct{
	encoder_as5047_t encoder;
	encoder_connected_t encoder_connected;
	long encoder_absolute_pos;
	float last_angle;
	float new_angle;
	float abs_angle;
	int turn_cnt;
	encoder_end_t end;
	double steps;
}koruza_encoder_t;

typedef struct{
	koruza_encoder_t encoder_x;
	koruza_encoder_t encoder_y;

}koruza_encoders_t;

extern koruza_encoders_t koruza_encoders;

void koruza_encoder_check(koruza_encoders_t *encoders);
void koruza_encoders_init(koruza_encoders_t *encoders, encoder_connected_t encoder_x_con, encoder_connected_t encoder_y_con);
void koruza_encoders_get_angles(koruza_encoders_t *encoders);
void koruza_encoders_get_all_data(koruza_encoders_t *encoders);
void koruza_encoders_absolute_position(koruza_encoders_t *encoders);
void koruza_encoders_absolute_position_steps(koruza_encoders_t *encoders);
//void koruza_encoders_end_position(koruza_encoders_t *encoders, koruza_steppers_t *steppers);
#endif /* ENCODER_H_ */
