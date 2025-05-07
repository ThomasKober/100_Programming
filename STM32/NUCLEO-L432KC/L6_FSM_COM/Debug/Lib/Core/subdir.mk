################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Lib/Core/Com.c \
../Lib/Core/Core.c 

OBJS += \
./Lib/Core/Com.o \
./Lib/Core/Core.o 

C_DEPS += \
./Lib/Core/Com.d \
./Lib/Core/Core.d 


# Each subdirectory must supply rules for building sources it contributes
Lib/Core/%.o Lib/Core/%.su Lib/Core/%.cyclo: ../Lib/Core/%.c Lib/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L432xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/1zwer/Documents/Projects/100_Programming/STM32/NUCLEO-L432KC/L6_FSM_COM/Lib/Core" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Lib-2f-Core

clean-Lib-2f-Core:
	-$(RM) ./Lib/Core/Com.cyclo ./Lib/Core/Com.d ./Lib/Core/Com.o ./Lib/Core/Com.su ./Lib/Core/Core.cyclo ./Lib/Core/Core.d ./Lib/Core/Core.o ./Lib/Core/Core.su

.PHONY: clean-Lib-2f-Core

