################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/AS4047D.c \
../src/AccelStepper.c \
../src/IRremote.c \
../src/WS2812B.c \
../src/adc.c \
../src/bootloader.c \
../src/crc32.c \
../src/dma.c \
../src/eeprom.c \
../src/encoder.c \
../src/frame.c \
../src/gpio.c \
../src/inet.c \
../src/irRecv.c \
../src/irSend.c \
../src/ir_Sony.c \
../src/ir_link.c \
../src/koruza_eeprom.c \
../src/main.c \
../src/message.c \
../src/spi.c \
../src/stepper.c \
../src/stm32f4xx_hal_msp.c \
../src/stm32f4xx_it.c \
../src/syscalls.c \
../src/system_stm32f4xx.c \
../src/tim.c \
../src/uart.c 

OBJS += \
./src/AS4047D.o \
./src/AccelStepper.o \
./src/IRremote.o \
./src/WS2812B.o \
./src/adc.o \
./src/bootloader.o \
./src/crc32.o \
./src/dma.o \
./src/eeprom.o \
./src/encoder.o \
./src/frame.o \
./src/gpio.o \
./src/inet.o \
./src/irRecv.o \
./src/irSend.o \
./src/ir_Sony.o \
./src/ir_link.o \
./src/koruza_eeprom.o \
./src/main.o \
./src/message.o \
./src/spi.o \
./src/stepper.o \
./src/stm32f4xx_hal_msp.o \
./src/stm32f4xx_it.o \
./src/syscalls.o \
./src/system_stm32f4xx.o \
./src/tim.o \
./src/uart.o 

C_DEPS += \
./src/AS4047D.d \
./src/AccelStepper.d \
./src/IRremote.d \
./src/WS2812B.d \
./src/adc.d \
./src/bootloader.d \
./src/crc32.d \
./src/dma.d \
./src/eeprom.d \
./src/encoder.d \
./src/frame.d \
./src/gpio.d \
./src/inet.d \
./src/irRecv.d \
./src/irSend.d \
./src/ir_Sony.d \
./src/ir_link.d \
./src/koruza_eeprom.d \
./src/main.d \
./src/message.d \
./src/spi.d \
./src/stepper.d \
./src/stm32f4xx_hal_msp.d \
./src/stm32f4xx_it.d \
./src/syscalls.d \
./src/system_stm32f4xx.d \
./src/tim.d \
./src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F4 -DSTM32 -DNUCLEO_F411RE -DSTM32F411RETx -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -I"C:/Users/vojis/Documents/stm_projects/koruza_driver_firmware/inc" -I"C:/Users/vojis/Documents/stm_projects/koruza_driver_firmware/CMSIS/core" -I"C:/Users/vojis/Documents/stm_projects/koruza_driver_firmware/CMSIS/device" -I"C:/Users/vojis/Documents/stm_projects/koruza_driver_firmware/HAL_Driver/Inc/Legacy" -I"C:/Users/vojis/Documents/stm_projects/koruza_driver_firmware/HAL_Driver/Inc" -I"C:/Users/vojis/Documents/stm_projects/koruza_driver_firmware/Utilities/STM32F4xx-Nucleo" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


