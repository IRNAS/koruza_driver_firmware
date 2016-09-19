/*
 * AS4047D.h
 *
 *  Created on: 24. avg. 2016
 *      Author: user
 */

#ifndef AS4047D_H_
#define AS4047D_H_

#include <stdint.h>
#include "gpio.h"

// AS4047D Register Addresses
typedef struct{
	uint16_t ERRFL;
	uint16_t PROG;
	uint16_t DIAAGC;
	uint16_t ANGLEUNC;
	uint16_t NOP;
	uint16_t CORDICMAG;
	uint16_t ANGLECOM;
	uint16_t ZPOSM;
	uint16_t ZPOSL;
	uint16_t SETTINGS1;
	uint16_t SETTINGS2;
	float    true_angle;
	uint16_t CS_pin;
	GPIO_TypeDef *CS_port;
}encoder_as5047_t;



/** Volatile registers**/
/* No operation register */
#define AS4047D_NOP 0x0000
/* Error register */
#define AS4047D_ERRFL 0x0001
/* Programming register */
#define AS4047D_PROG 0x0003
/* Diagnostic and AGC */
/* Default value: 0x0180 */
#define AS4047D_DIAAGC 0x3FFC
/* CORDIC magnitude */
#define AS4047D_CORDICMAG 0x3FFD
/* Measured angle without dynamic angle error compensation */
#define AS4047D_ANGLEUNC 0x3FFE
/* Measured angle with dynamic angle error compensation */
#define AS4047D_ANGLECOM 0x3FFF

/** Non-volatile registers **/
/* Zero position MSB */
#define AS4047D_ZPOSM 0x0016
/* Zero position LSB/MAG diagnostic */
#define AS4047D_ZPOSL 0x0017
/* Custom setting register 1 */
/* Default value: 0x0001 */
#define AS4047D_SETTINGS1 0x0018
/* Custom setting register 2 */
#define AS4047D_SETTINGS2 0x0019

#define AS4047D_RD 0x4000    // bit 14 = "1" is Read + parity even
#define AS4047D_WR 0x3FFF    // bit 14 = "0" is Write


extern void AS5047D_Init(encoder_as5047_t *encoder);

extern void AS5047D_Write(GPIO_TypeDef* CS_GPIO_Port, uint16_t CS_GPIO_Pin, uint16_t address, uint16_t data);
extern uint16_t AS5047D_Read(GPIO_TypeDef* CS_GPIO_Port, uint16_t CS_GPIO_Pin, uint16_t address);

extern void AS5047D_Check_Transmission_Error(encoder_as5047_t *encoder);
extern void AS5047D_SetZero(encoder_as5047_t *encoder);
extern uint16_t AS5047D_GetZero(encoder_as5047_t *encoder);
extern uint8_t AS5047D_Get_AGC_Value(encoder_as5047_t *encoder);

extern uint16_t AS5047D_Get_CORDICMAG_Value(encoder_as5047_t *encoder);
extern uint16_t AS5047D_Get_ANGLEUNC_Value(encoder_as5047_t *encoder);
extern uint16_t AS5047D_Get_ANGLECOM_Value(encoder_as5047_t *encoder);

extern float AS5047D_Get_True_Angle_Value(encoder_as5047_t *encoder);
extern void AS5047D_Get_All_Data(encoder_as5047_t *encoder);
/*This function enables the contribution of MAGH and MAGL (magnetic field strength too high and too low) to the error flag*/
void AS5047D_enable_MAG(encoder_as5047_t *encoder);
/* Magnetic field strength too low; AGC=0xFF */
/*  Magnetic field strength too high; AGC=0x00 */
int AS5047D_check_MAG(encoder_as5047_t *encoder);
/* Check if the encoder is connected */
int AS5047D_check_encoder(encoder_as5047_t *encoder);


#define AS5047D_Check_MAG_TooLow(DIAAGC)      ((DIAAGC >> 11) & 0x0001)
#define AS5047D_Check_MAG_TooHigh(DIAAGC)     ((DIAAGC >> 10) & 0x0001)
#define AS5047D_Check_CORDIC_Overflow(DIAAGC) ((DIAAGC >> 9)  & 0x0001)
#define AS5047D_Check_LF_finished(DIAAGC)     ((DIAAGC >> 8)  & 0x0001)

#endif /* AS4047D_H_ */
