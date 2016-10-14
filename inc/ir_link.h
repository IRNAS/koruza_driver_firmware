/*
 * ir_link.h
 *
 *  Created on: 14. okt. 2016
 *      Author: vojis
 */

#ifndef IR_LINK_H_
#define IR_LINK_H_

#include "IRremote.h"
#include "IRremoteInt.h"

typedef struct{
	irparams_t *irlink;
	ir_decode_results results;
}koruza_irlink_t;

extern koruza_irlink_t koruza_irlink;

void koruza_irlink_init(koruza_irlink_t *irlink);
void koruza_irlink_send(koruza_irlink_t *irlink, unsigned long data, int nbits);
void koruza_irlink_receive(koruza_irlink_t *irlink);

#endif /* IR_LINK_H_ */
