/*
 * WS2812B.c
 *
 * Created on: 13. okt. 2016
 * Author: Matej
 */

#include "gpio.h"
#include "WS2812B.h"

extern void Error_Handler(void);

static void WS2812B_send_0(void)
{
	LED_sdata_GPIO_Port->BSRR = LED_sdata_Pin;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");

	LED_sdata_GPIO_Port->BSRR = (uint32_t)LED_sdata_Pin << 16U;
	for(int indx = 0; indx < 10; indx++)
	{
		asm("nop");
	}
}

static void WS2812B_send_1(void)
{
	LED_sdata_GPIO_Port->BSRR = LED_sdata_Pin;
	for(int indx = 0; indx < 10; indx++)
	{
		asm("nop");
	}

	LED_sdata_GPIO_Port->BSRR = (uint32_t)LED_sdata_Pin << 16U;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
}

static void WS2812B_send_RET(void)
{
	LED_sdata_GPIO_Port->BSRR = (uint32_t)LED_sdata_Pin << 16U;
	for(int indx = 0; indx < 1000; indx++)
	{
		asm("nop");
	}
}

void WS2812B_send_packet(WS2812B_color_t* packet, uint32_t length)
{
	 /* Read PRIMASK register, check interrupt status before you disable them */
	 /* Returns 0 if they are enabled, or non-zero if disabled */
	 uint32_t prim = __get_PRIMASK();

	 /* Disable interrupts */
	 __disable_irq();

	WS2812B_send_RET();

	for(uint32_t i = 0; i < length; i++)
	{
		for(uint32_t j = 0; j < 8; j++)
		{
			if((packet[i].green >> j) & 0x01) WS2812B_send_1();
			else WS2812B_send_0();
		}

		for(uint32_t j = 0; j < 8; j++)
		{
			if((packet[i].red >> j) & 0x01) WS2812B_send_1();
			else WS2812B_send_0();
		}

		for(uint32_t j = 0; j < 8; j++)
		{
			if((packet[i].blue >> j) & 0x01) WS2812B_send_1();
			else WS2812B_send_0();
		}
	}

	WS2812B_send_RET();

	/* Enable interrupts back */
	 if (!prim) {
		 __enable_irq();
	 }
}

void WS2812B_level_indicator(uint32_t strength, uint32_t length)
{
	if(strength > length) Error_Handler();

	WS2812B_color_t packet[length];

	for(int i = 0; i < strength; i++)
	{
		packet[i].green = 0x00;
		packet[i].red = 0xFF;
		packet[i].blue = 0x00;
	}

	for(int i = strength; i < length; i++)
	{
		packet[i].green = 0xFF;
		packet[i].red = 0x00;
		packet[i].blue = 0x00;
	}

	WS2812B_send_packet(packet, length);
}

void WS2812B_level_indicator_wLED(WS2812B_color_t special_LED_color, uint32_t strength, uint32_t length)
{
	if(strength > length) Error_Handler();

	WS2812B_color_t packet[length + 1];

	for(int i = 0; i < strength; i++)
	{
		packet[i].green = 0x00;
		packet[i].red = 0xFF;
		packet[i].blue = 0x00;
	}

	for(int i = strength; i < length; i++)
	{
		packet[i].green = 0xFF;
		packet[i].red = 0x00;
		packet[i].blue = 0x00;
	}

	packet[length] = special_LED_color;

	WS2812B_send_packet(packet, length + 1);
}

void koruza_led_ring_calc(uint16_t rx_power, int *led_ring_num){
    int i = 0;
    if(rx_power < 10000 && rx_power > 0){
        for(i = 1; i < 24; i++){
            if(rx_power < i*400){
               *led_ring_num = i;
               break;
            }
        }
    }else{
        /* False data*/
        *led_ring_num = 0;
    }
}

void WS2812B_level_indicator_color(uint16_t rx_power, uint32_t length){
	int led_num = 0;
	//Devide by 10000 because it is mul when sent form witi
	//rx_power /= 10000;

	//if(rx_power > MAX_RX_POWER_mW) return;

	WS2812B_color_t packet[length];

	for(int i = 0; i < length; i++){
		// First 5x green
		if(i < 5){
			packet[i].green = 0xFF;
			packet[i].red = 0x00;
			packet[i].blue = 0x00;
		}
		// Second 5x cyan
		else if(i < 10){
			packet[i].green = 0xFF;
			packet[i].red = 0x00;
			packet[i].blue = 0xFF;
		}
		// Third 4x blue
		else if(i < 14){
			packet[i].green = 0x00;
			packet[i].red = 0x00;
			packet[i].blue = 0xFF;
		}
		// Fourth 5x purple
		else if(i < 19){
			packet[i].green = 0x00;
			packet[i].red = 0xFF;
			packet[i].blue = 0xFF;
		}
		// Fifth 5x red
		else if(i < 24){
			packet[i].green = 0x00;
			packet[i].red = 0xFF;
			packet[i].blue = 0x00;
		}
	}

	koruza_led_ring_calc(rx_power, &led_num);

	for(int i = led_num; i < length; i++){
			packet[i].green = 0x00;
			packet[i].red = 0x00;
			packet[i].blue = 0x00;
	}


	WS2812B_send_packet(packet, length);
}

