################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/AccelStepper.c \
../src/crc32.c \
../src/frame.c \
../src/gpio.c \
../src/inet.c \
../src/main.c \
../src/message.c \
../src/stepper.c \
../src/stm32f4xx_hal_msp.c \
../src/stm32f4xx_it.c \
../src/syscalls.c \
../src/system_stm32f4xx.c \
../src/uart.c 

OBJS += \
./src/AccelStepper.o \
./src/crc32.o \
./src/frame.o \
./src/gpio.o \
./src/inet.o \
./src/main.o \
./src/message.o \
./src/stepper.o \
./src/stm32f4xx_hal_msp.o \
./src/stm32f4xx_it.o \
./src/syscalls.o \
./src/system_stm32f4xx.o \
./src/uart.o 

C_DEPS += \
./src/AccelStepper.d \
./src/crc32.d \
./src/frame.d \
./src/gpio.d \
./src/inet.d \
./src/main.d \
./src/message.d \
./src/stepper.d \
./src/stm32f4xx_hal_msp.d \
./src/stm32f4xx_it.d \
./src/syscalls.d \
./src/system_stm32f4xx.d \
./src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F4 -DSTM32 -DNUCLEO_F411RE -DSTM32F411RETx -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -I"C:/Users/vojis/Documents/stm_projects/koruza_driver/inc" -I"C:/Users/vojis/Documents/stm_projects/koruza_driver/CMSIS/core" -I"C:/Users/vojis/Documents/stm_projects/koruza_driver/CMSIS/device" -I"C:/Users/vojis/Documents/stm_projects/koruza_driver/HAL_Driver/Inc/Legacy" -I"C:/Users/vojis/Documents/stm_projects/koruza_driver/HAL_Driver/Inc" -I"C:/Users/vojis/Documents/stm_projects/koruza_driver/Utilities/STM32F4xx-Nucleo" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


