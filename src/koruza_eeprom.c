/*
 * koruza_eeprom.c
 *
 *  Created on: 27. sep. 2016
 *      Author: vojis
 */
#include "koruza_eeprom.h"

koruza_parameters_t koruza_eeprom_parameters;

//uint16_t VirtAddVarTab[NB_OF_VAR] = {0x5555, 0x6666, 0x7777};

void koruza_eeprom_init(void){
	/* Unlock the Flash Program Erase controller */
	HAL_FLASH_Unlock();
	/* EEPROM Init */
	if( EE_Init() != EE_OK)
	{
	//Error_Handler();
	}
}/*
void koruza_eeprom_deinit(void){

}
void koruza_parameters_set(koruza_parameters_t *parameters, koruza_encoders_t *encoders){
	parameters->x.encoder_amplitude = (uint16_t)encoders->encoder_x.calibration.amplitude;
	parameters->x.encoder_offset = (uint16_t)encoders->encoder_x.calibration.offset;
	parameters->x.encoder_start = (uint16_t)encoders->encoder_x.calibration.start;
}

void koruza_parameters_get(koruza_parameters_t *parameters, koruza_encoders_t *encoders){
	double temp_amplitude = 0;
	double temp_offset = 0;
	double temp_start = 0;

	temp_amplitude = (double)parameters->x.encoder_amplitude;
	encoders->encoder_x.calibration.amplitude = temp_amplitude;

	temp_offset = (double)parameters->x.encoder_offset;
	encoders->encoder_x.calibration.offset = temp_offset;

	temp_start = (double)parameters->x.encoder_start;
	encoders->encoder_x.calibration.start = temp_start;

}


void koruza_parameters_write(koruza_parameters_t *parameters){
	if((EE_WriteVariable(VirtAddVarTab[0],  parameters->x.encoder_amplitude)) != HAL_OK){
		//Error_Handler();
	}
	if((EE_WriteVariable(VirtAddVarTab[1],  parameters->x.encoder_offset)) != HAL_OK){
		//Error_Handler();
	}
	if((EE_WriteVariable(VirtAddVarTab[2],  parameters->x.encoder_start)) != HAL_OK){
		//Error_Handler();
	}
}
void koruza_parameters_read(koruza_parameters_t *parameters){
	if((EE_ReadVariable(VirtAddVarTab[0],  &parameters->x.encoder_amplitude)) != HAL_OK)
	{
		//Error_Handler();
	}
	if((EE_ReadVariable(VirtAddVarTab[1],  &parameters->x.encoder_offset)) != HAL_OK)
	{
		//Error_Handler();
	}
	if((EE_ReadVariable(VirtAddVarTab[2],  &parameters->x.encoder_start)) != HAL_OK)
	{
		//Error_Handler();
	}
}
*/
