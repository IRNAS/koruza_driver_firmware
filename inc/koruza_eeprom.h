/*
 * koruza_eeprom.h
 *
 *  Created on: 27. sep. 2016
 *      Author: vojis
 */

#ifndef KORUZA_EEPROM_H_
#define KORUZA_EEPROM_H_

#include "eeprom.h"
#include "encoder.h"


typedef struct{
	uint16_t encoder_start;
	uint16_t encoder_amplitude;
	uint16_t encoder_offset;
}parameters_t;

typedef struct{
	parameters_t x;
	parameters_t y;
}koruza_parameters_t;

extern koruza_parameters_t koruza_eeprom_parameters;


void koruza_eeprom_init(void);
void koruza_eeprom_deinit(void);
void koruza_parameters_set(koruza_parameters_t *parameters, koruza_encoders_t *encoders);
void koruza_parameters_get(koruza_parameters_t *parameters, koruza_encoders_t *encoders);
void koruza_parameters_write(koruza_parameters_t *parameters);
void koruza_parameters_read(koruza_parameters_t *parameters);


#endif /* KORUZA_EEPROM_H_ */
