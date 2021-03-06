/*
 * AS4047D.c
 *
 *  Created on: 24. avg. 2016
 *      Author: user
 */

#include "AS4047D.h"
#include <stdint.h>
#include "spi.h"
#include "gpio.h"


uint16_t parity(uint16_t x)
{
	uint16_t parity = 0;

	while(x != 0)
	{
		parity ^= x;
		x >>= 1;
	}

	return (parity & 0x1);
}

void AS5047D_Write(GPIO_TypeDef* CS_GPIO_Port, uint16_t CS_GPIO_Pin, uint16_t address, uint16_t data)
{
	if (parity(address & 0x3FFF) == 1) address = address | 0x8000; // set parity bit
	//address = address & (WR | 0x8000);  // its  a write command and don't change the parity bit (0x8000)

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_RESET);

	//HAL_Delay(1);

	if (HAL_SPI_Transmit(&hspi2, (uint8_t*) &address, 2, 100) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_SET);

	//HAL_Delay(1);

	if (parity(data & 0x3FFF) == 1) data = data | 0x8000; // set parity bit
	//data = data & (WR | 0x8000); // its a write command and don't change the parity bit (0x8000)

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_RESET);

	//HAL_Delay(1);

	if (HAL_SPI_Transmit(&hspi2, (uint8_t*) &data, 2, 100) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_SET);
}

uint16_t AS5047D_Read(GPIO_TypeDef* CS_GPIO_Port, uint16_t CS_GPIO_Pin, uint16_t address)
{
	if (parity(address | AS4047D_RD) == 1) address = address | 0x8000; // set parity bit
	address = address | AS4047D_RD; // it's a read command

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_RESET);

	//HAL_Delay(1);

	if (HAL_SPI_Transmit(&hspi2, (uint8_t*) &address, 1, 100) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_SET);

	//HAL_Delay(1);

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_RESET);

	//HAL_Delay(1);

	uint16_t data = 0;

	if (HAL_SPI_Receive(&hspi2, (uint8_t*) &data, 1, 100) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_SET);

	data = data & 0x3FFF;  // filter bits outside data, strip bit 14..15
	return data;
}

void AS5047D_Check_Transmission_Error(encoder_as5047_t *encoder)
{
	/** Check if transmission error **/
	if(AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_ERRFL) != 0)
	{
		Error_Handler();
	}
}
/*TODO: check this function, it does not work*/
void AS5047D_SetZero(encoder_as5047_t *encoder)
{
	/** Check diagnostics reg **/
	uint16_t DIAAGC = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_DIAAGC);
	//AS5047D_Check_Transmission_Error();
	//if((AS5047D_Check_MAG_TooLow(DIAAGC)) || (AS5047D_Check_MAG_TooHigh(DIAAGC)) || (AS5047D_Check_CORDIC_Overflow(DIAAGC)) || !(AS5047D_Check_LF_finished(DIAAGC)))
	//{
		//Error_Handler();
	//}

	/** Get uncompensated angle reg value **/
	uint16_t ANGLEUNC = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_ANGLEUNC);
	//AS5047D_Check_Transmission_Error();

	/** Write to zero pos regs **/
	AS5047D_Write(encoder->CS_port, encoder->CS_pin, AS4047D_ZPOSM, (ANGLEUNC >> 6) & 0x00FF);
	//AS5047D_Check_Transmission_Error();
	AS5047D_Write(encoder->CS_port, encoder->CS_pin, AS4047D_ZPOSL, ANGLEUNC & 0x003F);
	//AS5047D_Check_Transmission_Error();
}

uint16_t AS5047D_GetZero(encoder_as5047_t *encoder)
{
	uint16_t ZPOSM = 0;
	uint16_t ZPOSL = 0;

	ZPOSM = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_ZPOSM);
	//AS5047D_Check_Transmission_Error();
	ZPOSL = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_ZPOSL);
	//AS5047D_Check_Transmission_Error();

	return (((ZPOSM << 6) & 0x3FC0) | (ZPOSL & 0x003F));
}

uint8_t AS5047D_Get_AGC_Value(encoder_as5047_t *encoder)
{
	/** Read diagnostics reg **/
	uint16_t DIAAGC = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_DIAAGC);
	//AS5047D_Check_Transmission_Error();
	return (uint8_t)((DIAAGC >> 8) & 0x00FF);
}

void AS5047D_Init(encoder_as5047_t *encoder)
{
	/* Initiaize AS4047D */
	AS5047D_Write(encoder->CS_port, encoder->CS_pin, AS4047D_SETTINGS1, 0b00000101);
	//AS5047D_Check_Transmission_Error();
	AS5047D_Write(encoder->CS_port, encoder->CS_pin, AS4047D_SETTINGS2, 0b00000000);
	//AS5047D_Check_Transmission_Error();
}

uint16_t AS5047D_Get_CORDICMAG_Value(encoder_as5047_t *encoder)
{
	uint16_t CORDICMAG = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_CORDICMAG);
	//AS5047D_Check_Transmission_Error();
	return CORDICMAG;
}

uint16_t AS5047D_Get_ANGLEUNC_Value(encoder_as5047_t *encoder)
{
	uint16_t ANGLEUNC = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_ANGLEUNC);
	//AS5047D_Check_Transmission_Error();
	return ANGLEUNC;
}

uint16_t AS5047D_Get_ANGLECOM_Value(encoder_as5047_t *encoder)
{
	uint16_t ANGLECOM = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_ANGLECOM);
	//AS5047D_Check_Transmission_Error();
	return ANGLECOM;
}

float AS5047D_Get_True_Angle_Value(encoder_as5047_t *encoder)
{
	return((float)AS5047D_Get_ANGLEUNC_Value(encoder) * 360.0f / 16383.0f);
	//return((float)AS5047D_Get_ANGLECOM_Value() * 360.0f / 16383.0f);
}

void AS5047D_Get_All_Data(encoder_as5047_t *encoder){
		encoder->ERRFL = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_ERRFL);
		encoder->PROG = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_PROG);
		encoder->DIAAGC = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_DIAAGC);
		encoder->ANGLEUNC = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_ANGLEUNC);
		encoder->NOP = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_NOP);
		encoder->CORDICMAG = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_CORDICMAG);
		encoder->ANGLECOM = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_ANGLECOM);
		encoder->ZPOSM = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_ZPOSM);
		encoder->ZPOSL = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_ZPOSL);
		encoder->SETTINGS1 = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_SETTINGS1);
		encoder->SETTINGS2 = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_SETTINGS2);
		encoder->true_angle = AS5047D_Get_True_Angle_Value(encoder);

}

void AS5047D_enable_MAG(encoder_as5047_t *encoder){
	AS5047D_Write(encoder->CS_port, encoder->CS_pin, AS4047D_ZPOSL, 0b00000011);
}

int AS5047D_check_MAG(encoder_as5047_t *encoder){
	//encoder->DIAAGC = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_DIAAGC);
	AS5047D_Get_All_Data(encoder);
	if((encoder->DIAAGC & 0x00FF) == 0x00FF){
		return 0;
	}
	else if((encoder->DIAAGC & 0x00FF) == 0x0000){
		return 0;
	}
	else{
		return 1;
	}
}

int AS5047D_check_encoder(encoder_as5047_t *encoder){
	//encoder->SETTINGS1 = AS5047D_Read(encoder->CS_port, encoder->CS_pin, AS4047D_SETTINGS1);
	AS5047D_Get_All_Data(encoder);
	if((encoder->SETTINGS1 != 0) && (encoder->SETTINGS1 != 0x3fff)){
		return 1;
	}
	else{
		return 0;
	}
}
