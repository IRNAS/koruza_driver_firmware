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
#include "math.h"
#include "message.h"
//#include "stepper.h"

/* Uncomment to get debug messages in the UART2 terminal about encoder status */
#define DEBUG_ENCODER_MODE

/* Maximum angle to check when the full circle*/
#define MAX_DIF_ANGLE 200   //degrees
//#define ENCODER_END_DIF 20 //degrees

#define ENCODER_STEPPER_MAX_ERROR 1000

#define STEPS_PER_ROTATION 4096
#define ONE_ANGLE_STEPPS 11.37777777777778

#define CORRECTION_FACOTR 1.2
/* Encoder X port and pin for chip select*/
#define AS4047D_CS1_Port GPIOB
#define AS4047D_CS1_Pin GPIO_PIN_12

/* Encoder Y port and pin for chip select*/
#define AS4047D_CS2_Port GPIOC
#define AS4047D_CS2_Pin GPIO_PIN_6

// Converts degrees to radians.
#define degreesToRadians(angleDegrees) (angleDegrees * M_PI / 180.0)

// Converts radians to degrees.
#define radiansToDegrees(angleRadians) (angleRadians * 180.0 / M_PI)

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
	double start;
	double amplitude;
	double offset;
}encoder_calibration_parameters_t;

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
	encoder_calibration_parameters_t calibration;
	double diff;
}koruza_encoder_t;

typedef struct{
	koruza_encoder_t encoder_x;
	koruza_encoder_t encoder_y;

}koruza_encoders_t;

extern koruza_encoders_t koruza_encoders;
extern tlv_error_report_t koruza_error_report;
extern tlv_error_report_t koruza_error_report_ch;
extern uint32_t koruza_error_report_check;

void koruza_encoder_check(koruza_encoders_t *encoders);
void koruza_encoders_sin(koruza_encoder_t *encoder);
void koruza_encoders_init(koruza_encoders_t *encoders, encoder_connected_t encoder_x_con, encoder_connected_t encoder_y_con);
void koruza_encoders_get_angles(koruza_encoders_t *encoders);
void koruza_encoders_get_all_data(koruza_encoders_t *encoders);
void koruza_encoders_absolute_position(koruza_encoders_t *encoders);
void koruza_encoders_absolute_position_steps(koruza_encoders_t *encoders);
void koruza_encoders_magnetic_filed_check(uint32_t *new_error_report, koruza_encoders_t *encoders, uint8_t get_data);
//void koruza_encoders_end_position(koruza_encoders_t *encoders, koruza_steppers_t *steppers);
#endif /* ENCODER_H_ */
