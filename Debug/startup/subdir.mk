################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32f411xe.s 

OBJS += \
./startup/startup_stm32f411xe.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo %cd%
	arm-none-eabi-as -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -I"C:/Users/vojis/Documents/stm_projects/koruza_driver_firmware_1/inc" -I"C:/Users/vojis/Documents/stm_projects/koruza_driver_firmware_1/CMSIS/core" -I"C:/Users/vojis/Documents/stm_projects/koruza_driver_firmware_1/CMSIS/device" -I"C:/Users/vojis/Documents/stm_projects/koruza_driver_firmware_1/HAL_Driver/Inc/Legacy" -I"C:/Users/vojis/Documents/stm_projects/koruza_driver_firmware_1/HAL_Driver/Inc" -I"C:/Users/vojis/Documents/stm_projects/koruza_driver_firmware_1/Utilities/STM32F4xx-Nucleo" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


