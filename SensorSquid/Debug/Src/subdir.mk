################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
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
../Src/tim.c \
../Src/timestamps.c \
../Src/usart.c \
../Src/wheel_speed.c 

OBJS += \
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
./Src/tim.o \
./Src/timestamps.o \
./Src/usart.o \
./Src/wheel_speed.o 

C_DEPS += \
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
./Src/tim.d \
./Src/timestamps.d \
./Src/usart.d \
./Src/wheel_speed.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F446xx -I"C:/GitHub/SensorSquid/SensorSquid/Inc" -I"C:/GitHub/SensorSquid/SensorSquid/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/GitHub/SensorSquid/SensorSquid/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"C:/GitHub/SensorSquid/SensorSquid/Middlewares/Third_Party/FatFs/src" -I"C:/GitHub/SensorSquid/SensorSquid/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/GitHub/SensorSquid/SensorSquid/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/GitHub/SensorSquid/SensorSquid/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"C:/GitHub/SensorSquid/SensorSquid/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/GitHub/SensorSquid/SensorSquid/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


