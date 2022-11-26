################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/IMU_CAN.c \
../Src/adc.c \
../Src/bsp_driver_sd.c \
../Src/can.c \
../Src/dma.c \
../Src/fatfs.c \
../Src/fatfs_platform.c \
../Src/freertos.c \
../Src/gpio.c \
../Src/main.c \
../Src/rtc.c \
../Src/sd_card.c \
../Src/sd_diskio.c \
../Src/sdio.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_hal_timebase_tim.c \
../Src/stm32f4xx_it.c \
../Src/system_stm32f4xx.c \
../Src/thermistor.c \
../Src/tim.c \
../Src/timestamps.c \
../Src/usart.c \
../Src/wheel_speed.c 

OBJS += \
./Src/IMU_CAN.o \
./Src/adc.o \
./Src/bsp_driver_sd.o \
./Src/can.o \
./Src/dma.o \
./Src/fatfs.o \
./Src/fatfs_platform.o \
./Src/freertos.o \
./Src/gpio.o \
./Src/main.o \
./Src/rtc.o \
./Src/sd_card.o \
./Src/sd_diskio.o \
./Src/sdio.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_hal_timebase_tim.o \
./Src/stm32f4xx_it.o \
./Src/system_stm32f4xx.o \
./Src/thermistor.o \
./Src/tim.o \
./Src/timestamps.o \
./Src/usart.o \
./Src/wheel_speed.o 

C_DEPS += \
./Src/IMU_CAN.d \
./Src/adc.d \
./Src/bsp_driver_sd.d \
./Src/can.d \
./Src/dma.d \
./Src/fatfs.d \
./Src/fatfs_platform.d \
./Src/freertos.d \
./Src/gpio.d \
./Src/main.d \
./Src/rtc.d \
./Src/sd_card.d \
./Src/sd_diskio.d \
./Src/sdio.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_hal_timebase_tim.d \
./Src/stm32f4xx_it.d \
./Src/system_stm32f4xx.d \
./Src/thermistor.d \
./Src/tim.d \
./Src/timestamps.d \
./Src/usart.d \
./Src/wheel_speed.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -Og -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/IMU_CAN.d ./Src/IMU_CAN.o ./Src/IMU_CAN.su ./Src/adc.d ./Src/adc.o ./Src/adc.su ./Src/bsp_driver_sd.d ./Src/bsp_driver_sd.o ./Src/bsp_driver_sd.su ./Src/can.d ./Src/can.o ./Src/can.su ./Src/dma.d ./Src/dma.o ./Src/dma.su ./Src/fatfs.d ./Src/fatfs.o ./Src/fatfs.su ./Src/fatfs_platform.d ./Src/fatfs_platform.o ./Src/fatfs_platform.su ./Src/freertos.d ./Src/freertos.o ./Src/freertos.su ./Src/gpio.d ./Src/gpio.o ./Src/gpio.su ./Src/main.d ./Src/main.o ./Src/main.su ./Src/rtc.d ./Src/rtc.o ./Src/rtc.su ./Src/sd_card.d ./Src/sd_card.o ./Src/sd_card.su ./Src/sd_diskio.d ./Src/sd_diskio.o ./Src/sd_diskio.su ./Src/sdio.d ./Src/sdio.o ./Src/sdio.su ./Src/stm32f4xx_hal_msp.d ./Src/stm32f4xx_hal_msp.o ./Src/stm32f4xx_hal_msp.su ./Src/stm32f4xx_hal_timebase_tim.d ./Src/stm32f4xx_hal_timebase_tim.o ./Src/stm32f4xx_hal_timebase_tim.su ./Src/stm32f4xx_it.d ./Src/stm32f4xx_it.o ./Src/stm32f4xx_it.su ./Src/system_stm32f4xx.d ./Src/system_stm32f4xx.o ./Src/system_stm32f4xx.su ./Src/thermistor.d ./Src/thermistor.o ./Src/thermistor.su ./Src/tim.d ./Src/tim.o ./Src/tim.su ./Src/timestamps.d ./Src/timestamps.o ./Src/timestamps.su ./Src/usart.d ./Src/usart.o ./Src/usart.su ./Src/wheel_speed.d ./Src/wheel_speed.o ./Src/wheel_speed.su

.PHONY: clean-Src

