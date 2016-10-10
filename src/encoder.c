/*
 * encoder.c
 *
 *  Created on: 31. avg. 2016
 *      Author: vojis
 */

#include "encoder.h"
#include "stdlib.h"
#include <stdio.h>

koruza_encoders_t koruza_encoders;
tlv_error_report_t koruza_error_report;
tlv_error_report_t koruza_error_report_ch;
uint32_t koruza_error_report_check;

void koruza_encoder_check(koruza_encoders_t *encoders){
	/* Get angles form encoder X and encoder Y */
	koruza_encoders_get_angles(encoders);
	//koruza_encoders_sin(&encoders->encoder_x);
	/* Calculate absolute position of encoders */
	koruza_encoders_absolute_position(encoders);
	/* check stepper motor error */
	koruza_encoders_absolute_position_steps(encoders);

}

void koruza_encoders_sin(koruza_encoder_t *encoder){
	if(encoder->encoder_connected == CONNECTED){
		encoder->encoder.true_angle += (encoder->calibration.amplitude * sin(degreesToRadians((double)encoder->encoder.true_angle - encoder->calibration.start)) + encoder->calibration.offset)/ONE_ANGLE_STEPPS;
	}

}

void koruza_encoders_init(koruza_encoders_t *encoders, encoder_connected_t encoder_x_con, encoder_connected_t encoder_y_con){
	/* Initialize AS4047D */

	encoders->encoder_x.encoder_connected = encoder_x_con;
	encoders->encoder_y.encoder_connected = encoder_y_con;
	if(encoders->encoder_x.encoder_connected == CONNECTED){
		encoders->encoder_x.encoder.CS_port = AS4047D_CS1_Port;
		encoders->encoder_x.encoder.CS_pin = AS4047D_CS1_Pin;
		AS5047D_Get_All_Data(&encoders->encoder_x.encoder);
		AS5047D_Init(&encoders->encoder_x.encoder);
		AS5047D_SetZero(&encoders->encoder_x.encoder);
		if(AS5047D_check_encoder(&encoders->encoder_x.encoder) == 0){
			encoders->encoder_x.encoder_connected = NOT_CONNECTED;
#ifdef DEBUG_ENCODER_MODE
			printf("encoder X: NOT CONNECTED\n\n");
#endif
		}
		else{
#ifdef DEBUG_ENCODER_MODE
			printf("encoder X: CONNECTED\n");
#endif
			if((koruza_encoders.encoder_x.encoder.DIAAGC & 0x00FF) == 0x00FF){
#ifdef DEBUG_ENCODER_MODE
				printf("encoder X: Magnetic field strength too low\n");
				printf("encoder X: NOT CONNECTED - magnet problems\n");
#endif
				koruza_encoders.encoder_x.encoder_connected = NOT_CONNECTED;
			}
			else if((koruza_encoders.encoder_x.encoder.DIAAGC & 0x00FF) == 0x0000){
#ifdef DEBUG_ENCODER_MODE
				printf("encoder X: Magnetic field strength too high\n");
				printf("encoder X: NOT CONNECTED - magnet problems\n");
#endif
				koruza_encoders.encoder_x.encoder_connected = NOT_CONNECTED;
			}
			else{
#ifdef DEBUG_ENCODER_MODE
				printf("encoder X: Magnetic field strength OK\n");
#endif
			}
		}
	}
	if(encoders->encoder_y.encoder_connected == CONNECTED){
		encoders->encoder_y.encoder.CS_port = AS4047D_CS2_Port;
		encoders->encoder_y.encoder.CS_pin = AS4047D_CS2_Pin;
		AS5047D_Get_All_Data(&encoders->encoder_y.encoder);
		AS5047D_Init(&encoders->encoder_y.encoder);
		AS5047D_SetZero(&encoders->encoder_y.encoder);
		if(AS5047D_check_encoder(&encoders->encoder_y.encoder) == 0){
			encoders->encoder_y.encoder_connected = NOT_CONNECTED;
#ifdef DEBUG_ENCODER_MODE
			//printf("\n");
			printf("encoder Y not connected");
#endif
		}
		else{
#ifdef DEBUG_ENCODER_MODE
			printf("encoder Y: CONNECTED\n");
#endif
			if((koruza_encoders.encoder_y.encoder.DIAAGC & 0x00FF) == 0x00FF){
#ifdef DEBUG_ENCODER_MODE
				printf("encoder Y: Magnetic field strength too low\n");
				printf("encoder Y: NOT CONNECTED - magnet problems");
#endif
				koruza_encoders.encoder_y.encoder_connected = NOT_CONNECTED;
			}
			else if((koruza_encoders.encoder_y.encoder.DIAAGC & 0x00FF) == 0x0000){
#ifdef DEBUG_ENCODER_MODE
				printf("encoder Y: Magnetic field strength too high\n");
				printf("encoder Y: NOT CONNECTED - magnet problems");
#endif
				koruza_encoders.encoder_y.encoder_connected = NOT_CONNECTED;
			}
			else{
#ifdef DEBUG_ENCODER_MODE
				printf("encoder Y: Magnetic field strength OK\n");
#endif
			}
		}
	}
	encoders->encoder_x.end = ENCODER_RUN;
	encoders->encoder_y.end = ENCODER_RUN;
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
	else{
		encoders->encoder_x.encoder.true_angle = 0;
	}
	if(encoders->encoder_y.encoder_connected == CONNECTED){
		encoders->encoder_y.encoder.true_angle = koruza_encoders_get_y_angle(encoders);
	}
	else{
		encoders->encoder_y.encoder.true_angle = 0;
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
	float dif_angle_x = 0;
	float dif_angle_y = 0;

	/* Values of the angles are integer numbers without the decimal numbers */
	/* This will produce an error in the position, but it will still be accurate enough */
	encoders->encoder_x.new_angle = encoders->encoder_x.encoder.true_angle;
	encoders->encoder_y.new_angle = encoders->encoder_y.encoder.true_angle;

	if(encoders->encoder_x.encoder_connected == CONNECTED){
		if(labs((long)encoders->encoder_x.new_angle - (long)encoders->encoder_x.last_angle) > MAX_DIF_ANGLE){
			if((long)encoders->encoder_x.new_angle - (long)encoders->encoder_x.last_angle < 0){
				//encoders->encoder_x.turn_cnt--;
				encoders->encoder_x.turn_cnt++;
			}
			else{
				//encoders->encoder_x.turn_cnt++;
				encoders->encoder_x.turn_cnt--;
			}
		}
		/* Calculate positive direction dif_angle */
		if(encoders->encoder_x.turn_cnt >= 0){
			//dif_angle_x = encoders->encoder_x.turn_cnt * 360 + (360 - encoders->encoder_x.new_angle) - encoders->encoder_x.abs_angle;
			dif_angle_x = encoders->encoder_x.turn_cnt * 360 + encoders->encoder_x.new_angle - encoders->encoder_x.abs_angle;

		}
		/* Calculate negative direction dif_angle */
		else{
			//dif_angle_x = (abs(encoders->encoder_x.turn_cnt)-1) * 360 + encoders->encoder_x.new_angle - encoders->encoder_x.abs_angle;
			dif_angle_x = (abs(encoders->encoder_x.turn_cnt)-1) * 360 + (360 - encoders->encoder_x.new_angle) - encoders->encoder_x.abs_angle;

		}
		encoders->encoder_x.abs_angle += dif_angle_x;
	}
	if(encoders->encoder_y.encoder_connected == CONNECTED){
		if(labs((long)(encoders->encoder_y.new_angle - (long)encoders->encoder_y.last_angle)) > MAX_DIF_ANGLE){
			if((long)encoders->encoder_y.new_angle - encoders->encoder_y.last_angle < 0){
				//encoders->encoder_y.turn_cnt--;
				encoders->encoder_y.turn_cnt++;

			}
			else{
				//encoders->encoder_y.turn_cnt++;
				encoders->encoder_y.turn_cnt--;

			}
		}
		/* Calculate positive direction dif_angle */
		if(encoders->encoder_y.turn_cnt >= 0){
			//dif_angle_y = encoders->encoder_y.turn_cnt * 360 + (360 - encoders->encoder_y.new_angle) - encoders->encoder_y.abs_angle;
			dif_angle_y = encoders->encoder_y.turn_cnt * 360 + encoders->encoder_y.new_angle - encoders->encoder_y.abs_angle;

		}
		/* Calculate negative direction dif_angle */
		else{
			//dif_angle_y = (abs(encoders->encoder_y.turn_cnt)-1) * 360 + encoders->encoder_y.new_angle - encoders->encoder_y.abs_angle;
			dif_angle_y = (abs(encoders->encoder_y.turn_cnt)-1) * 360 + (360 - encoders->encoder_y.new_angle) - encoders->encoder_y.abs_angle;

		}

		encoders->encoder_y.abs_angle += dif_angle_y;
	}

	encoders->encoder_x.last_angle = encoders->encoder_x.new_angle;
	encoders->encoder_y.last_angle = encoders->encoder_y.new_angle;
}

void koruza_encoders_absolute_position_steps(koruza_encoders_t *encoders){
	if(encoders->encoder_x.encoder_connected == CONNECTED){
		if(encoders->encoder_x.turn_cnt >= 0){
			encoders->encoder_x.steps = encoders->encoder_x.abs_angle * ONE_ANGLE_STEPPS;
		}
		else{
			encoders->encoder_x.steps = 0 - (encoders->encoder_x.abs_angle * ONE_ANGLE_STEPPS);
		}
	}
	if(encoders->encoder_y.encoder_connected == CONNECTED){
		if(encoders->encoder_y.turn_cnt >= 0){
			encoders->encoder_y.steps = encoders->encoder_y.abs_angle * ONE_ANGLE_STEPPS;
		}
		else{
			encoders->encoder_y.steps = 0 - (encoders->encoder_y.abs_angle * ONE_ANGLE_STEPPS);
		}
	}
}

void koruza_encoders_magnetic_filed_check(uint32_t *new_error_report, koruza_encoders_t *encoders, uint8_t get_data){
	/* Get data from encoders */
	if(get_data == 1){
		if(encoders->encoder_x.encoder_connected == CONNECTED){
			AS5047D_Get_All_Data(&encoders->encoder_x.encoder);
		}
		if(encoders->encoder_y.encoder_connected == CONNECTED){
			AS5047D_Get_All_Data(&encoders->encoder_y.encoder);
		}
	}

	if(encoders->encoder_x.encoder_connected == CONNECTED){
		*new_error_report &= ~(1 << 0);
	}else{
		*new_error_report |= 1 << 0;
	}
	if(encoders->encoder_y.encoder_connected == CONNECTED){
		*new_error_report &= ~(1 << 1);
	}else{
		*new_error_report |= 1 << 1;
	}
	if((encoders->encoder_x.encoder.DIAAGC & 0x00FF) == 0x00FF){
		/* Magnetic field X too low */
		*new_error_report |= 1 << 2;
	}else if ((encoders->encoder_x.encoder.DIAAGC & 0x00FF) == 0x0000){
		/* Magnetic field X too high */
		*new_error_report |= 1 << 4;
	}else{
		/* Magnetic field X OK */
		*new_error_report &= ~(1 << 2);
		*new_error_report &= ~(1 << 4);
	}
	if((encoders->encoder_y.encoder.DIAAGC & 0x00FF) == 0x00FF){
		/* Magnetic field X too low */
		*new_error_report |= 1 << 3;
	}else if ((encoders->encoder_y.encoder.DIAAGC & 0x00FF) == 0x0000){
		/* Magnetic field X too high */
		*new_error_report |= 1 << 5;
	}else{
		/* Magnetic field X OK */
		*new_error_report &= ~(1 << 3);
		*new_error_report &= ~(1 << 5);
	}
}

