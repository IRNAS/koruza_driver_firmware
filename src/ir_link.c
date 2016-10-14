/*
 * ir_link.c
 *
 *  Created on: 14. okt. 2016
 *      Author: vojis
 */
#include "ir_link.h"
#include "stdlib.h"
#include <stdio.h>

koruza_irlink_t koruza_irlink;


void koruza_irlink_init(koruza_irlink_t *irlink){
	//*irlink->irlink = &irparams;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	IRrecv_IRrecvInit(GPIOB, GPIO_PIN_4);
	IRrecv_enableIRIn(); // Start the receiver
}

// nbits = 12
void koruza_irlink_send(koruza_irlink_t *irlink, unsigned long data, int nbits){
	for (int i = 0; i < 3; i++)
	{
		IRsend_sendSony(data, nbits);
		HAL_Delay(4000); //400ms delay
	}
}

void koruza_irlink_receive(koruza_irlink_t *irlink){
	if (IRrecv_decode(&irlink->results))
	{
		//TODO: when received than what?
		//xyz = results.value;
		printf("got signal %#08x\n", (unsigned int)irlink->results.value);
	    IRrecv_resume(); // Receive the next value
	}
}
