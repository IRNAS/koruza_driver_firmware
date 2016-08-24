/*
 * bootloader.h
 *
 *  Created on: 24. avg. 2016
 *      Author: vojis
 */

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#include "stm32f4xx_hal.h"

#define BOOTLOADER_START_ADDR 0x1FFF0000

void JumpToBootLoader(void);

#endif /* BOOTLOADER_H_ */
