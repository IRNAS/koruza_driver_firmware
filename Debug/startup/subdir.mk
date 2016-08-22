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
	arm-none-eabi-as -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -I"C:/Users/vojis/workspace/koruza_driver_firmware/inc" -I"C:/Users/vojis/workspace/koruza_driver_firmware/CMSIS/core" -I"C:/Users/vojis/workspace/koruza_driver_firmware/CMSIS/device" -I"C:/Users/vojis/workspace/koruza_driver_firmware/HAL_Driver/Inc/Legacy" -I"C:/Users/vojis/workspace/koruza_driver_firmware/HAL_Driver/Inc" -I"C:/Users/vojis/workspace/koruza_driver_firmware/Utilities/STM32F4xx-Nucleo" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


