################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/bsp_driver_sd.c \
../Src/fatfs.c \
../Src/freertos.c \
../Src/main.c \
../Src/sd_diskio.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_it.c \
../Src/system_stm32f4xx.c 

OBJS += \
./Src/bsp_driver_sd.o \
./Src/fatfs.o \
./Src/freertos.o \
./Src/main.o \
./Src/sd_diskio.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_it.o \
./Src/system_stm32f4xx.o 

C_DEPS += \
./Src/bsp_driver_sd.d \
./Src/fatfs.d \
./Src/freertos.d \
./Src/main.d \
./Src/sd_diskio.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_it.d \
./Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F446xx -I"C:/GitHub/SensorSquid/SensorSquid/Inc" -I"C:/GitHub/SensorSquid/SensorSquid/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/GitHub/SensorSquid/SensorSquid/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"C:/GitHub/SensorSquid/SensorSquid/Middlewares/Third_Party/FatFs/src" -I"C:/GitHub/SensorSquid/SensorSquid/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/GitHub/SensorSquid/SensorSquid/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/GitHub/SensorSquid/SensorSquid/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"C:/GitHub/SensorSquid/SensorSquid/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/GitHub/SensorSquid/SensorSquid/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


