/*
 * encoder.c
 *
 *  Created on: 31. avg. 2016
 *      Author: vojis
 */

#include "encoder.h"
#include "stdlib.h"

koruza_encoders_t koruza_encoders;

void koruza_encoders_init(koruza_encoders_t *encoders, encoder_connected_t encoder_x_con, encoder_connected_t encoder_y_con){
	/* Initialize AS4047D */
	encoders->encoder_x.encoder_connected = encoder_x_con;
	encoders->encoder_y.encoder_connected = encoder_y_con;
	if(encoders->encoder_x.encoder_connected == CONNECTED){
		encoders->encoder_x.encoder.CS_port = AS4047D_CS1_Port;
		encoders->encoder_x.encoder.CS_pin = AS4047D_CS1_Pin;
		AS5047D_Init(&encoders->encoder_x.encoder);
		AS5047D_SetZero(&encoders->encoder_x.encoder);
	}
	if(encoders->encoder_y.encoder_connected == CONNECTED){
		encoders->encoder_y.encoder.CS_port = AS4047D_CS2_Port;
		encoders->encoder_y.encoder.CS_pin = AS4047D_CS2_Pin;
		AS5047D_Init(&encoders->encoder_y.encoder);
		AS5047D_SetZero(&encoders->encoder_y.encoder);
	}
}
static float koruza_encoders_get_x_angle(koruza_encoders_t *encoders){
	return AS5047D_Get_True_Angle_Value(&encoders->encoder_x.encoder);
}
static float koruza_encoders_get_y_angle(koruza_encoders_t *encoders){
	return AS5047D_Get_True_Angle_Value(&encoders->encoder_y.encoder);
}
void koruza_encoders_get_angles(koruza_encoders_t *encoders){
	if(encoders->encoder_x.encoder_connected == CONNECTED){
		encoders->encoder_x.encoder.true_angle = koruza_encoders_get_x_angle(encoders);
	}
	if(encoders->encoder_y.encoder_connected == CONNECTED){
		encoders->encoder_y.encoder.true_angle = koruza_encoders_get_y_angle(encoders);
	}
}
void koruza_encoders_get_all_data(koruza_encoders_t *encoders){
	if(encoders->encoder_x.encoder_connected == CONNECTED){
		AS5047D_Get_All_Data(&encoders->encoder_x.encoder);
	}
	if(encoders->encoder_y.encoder_connected == CONNECTED){
		AS5047D_Get_All_Data(&encoders->encoder_y.encoder);
	}
}

void koruza_encoders_absolute_position(koruza_encoders_t *encoders){
	long dif_angle_x = 0;
	long dif_angle_y = 0;

	/* Values of the angles are integer numbers without the decimal numbers */
	/* This will produce an error in the position, but it will still be accurate enough */
	encoders->encoder_x.new_angle = (long)(encoders->encoder_x.encoder.true_angle);
	encoders->encoder_y.new_angle = (long)(encoders->encoder_y.encoder.true_angle);

	if(encoders->encoder_x.encoder_connected == CONNECTED){
		if(labs(encoders->encoder_x.new_angle - encoders->encoder_x.last_angle) > MAX_DIF_ANGLE){
			if(encoders->encoder_x.new_angle - encoders->encoder_x.last_angle < 0){
				encoders->encoder_x.turn_cnt++;
			}
			else{
				encoders->encoder_x.turn_cnt--;
			}
		}
		dif_angle_x = encoders->encoder_x.turn_cnt * 360 + encoders->encoder_x.new_angle - encoders->encoder_x.abs_angle;
		encoders->encoder_x.abs_angle += dif_angle_x;
	}
	if(encoders->encoder_y.encoder_connected == CONNECTED){
		if(labs((long)(encoders->encoder_y.new_angle - encoders->encoder_y.last_angle)) > MAX_DIF_ANGLE){
			if(encoders->encoder_y.new_angle - encoders->encoder_y.last_angle < 0){
				encoders->encoder_y.turn_cnt++;
			}
			else{
				encoders->encoder_y.turn_cnt--;
			}
		}
		dif_angle_y = encoders->encoder_y.turn_cnt * 360 + encoders->encoder_y.new_angle - encoders->encoder_y.abs_angle;
		encoders->encoder_y.abs_angle += dif_angle_y;
	}

	encoders->encoder_x.last_angle = encoders->encoder_x.new_angle;
	encoders->encoder_y.last_angle = encoders->encoder_y.new_angle;
}

int  koruza_encoder_end(koruza_encoder_t encoder){
	/*check if the ecoder */
	//encoder.abs_angle
}
