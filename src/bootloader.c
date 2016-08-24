/*
 * bootloader.c
 *
 *  Created on: 24. avg. 2016
 *      Author: vojis
 */

#include "bootloader.h"

void JumpToBootLoader()
{
	HAL_RCC_DeInit(); // disable all pheripheral clocks
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;

	__set_PRIMASK(1); // disable interrupts

    __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();
    __set_MSP(*(__IO uint32_t*) 0x1FFF0000);
    void (*bootloader)(void) = (void(*)(void)) *((uint32_t *) (BOOTLOADER_START_ADDR + 4));
    bootloader();
}
