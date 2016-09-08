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
//TODO: proveri koji bi ovo bio ugao
#define MAX_DIF_ANGLE 50   //degrees
#define ENCODER_END_DIF 20 //degrees

/* Encoder X port and pin for chip select*/
#define AS4047D_CS1_Port GPIOB
#define AS4047D_CS1_Pin GPIO_PIN_12

/* Encoder Y port and pin for chip select*/
#define AS4047D_CS2_Port GPIOC
#define AS4047D_CS2_Pin GPIO_PIN_6

typedef enum{
	NOT_CONNECTED = 0,
	CONNECTED = 1,
}encoder_connected_t;

typedef struct{
	encoder_as5047_t encoder;
	encoder_connected_t encoder_connected;
	long encoder_absolute_pos;
	long last_angle;
	long new_angle;
	long abs_angle;
	int turn_cnt;
}koruza_encoder_t;

typedef struct{
	koruza_encoder_t encoder_x;
	koruza_encoder_t encoder_y;

}koruza_encoders_t;

extern koruza_encoders_t koruza_encoders;

void koruza_encoders_init(koruza_encoders_t *encoders, encoder_connected_t encoder_x_con, encoder_connected_t encoder_y_con);
void koruza_encoders_get_angles(koruza_encoders_t *encoders);
void koruza_encoders_get_all_data(koruza_encoders_t *encoders);
void koruza_encoders_absolute_position(koruza_encoders_t *encoders);
int  koruza_encoder_end(koruza_encoder_t encoder);

#endif /* ENCODER_H_ */
