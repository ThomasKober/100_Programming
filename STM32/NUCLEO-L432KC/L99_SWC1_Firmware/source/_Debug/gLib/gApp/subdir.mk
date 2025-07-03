################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../gLib/gApp/gApp.c \
../gLib/gApp/gVib.c \
../gLib/gApp/mp8859.c 

OBJS += \
./gLib/gApp/gApp.o \
./gLib/gApp/gVib.o \
./gLib/gApp/mp8859.o 

C_DEPS += \
./gLib/gApp/gApp.d \
./gLib/gApp/gVib.d \
./gLib/gApp/mp8859.d 


# Each subdirectory must supply rules for building sources it contributes
gLib/gApp/%.o gLib/gApp/%.su gLib/gApp/%.cyclo: ../gLib/gApp/%.c gLib/gApp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H562xx -c -I../Core/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../Drivers/CMSIS/Include -I../gLib/gCore -I../gLib/gApp -I../Middlewares/Third_Party/gtec_Driver/gCore/crypto/cipher -I../Middlewares/Third_Party/gtec_Driver/gCore/crypto -I../Middlewares/Third_Party/gtec_Driver/gCore/crypto/drbg -I../Middlewares/Third_Party/gtec_Driver/gCore/crypto/ecc -I../Middlewares/Third_Party/gtec_Driver/gCore/crypto/hash -I../Middlewares/Third_Party/gtec_Driver/gCore/crypto/mac -I../Middlewares/Third_Party/gtec_Driver/gCore/crypto/rsa -I../Middlewares/Third_Party/gtec_Driver/gCore/crypto/utils -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-gLib-2f-gApp

clean-gLib-2f-gApp:
	-$(RM) ./gLib/gApp/gApp.cyclo ./gLib/gApp/gApp.d ./gLib/gApp/gApp.o ./gLib/gApp/gApp.su ./gLib/gApp/gVib.cyclo ./gLib/gApp/gVib.d ./gLib/gApp/gVib.o ./gLib/gApp/gVib.su ./gLib/gApp/mp8859.cyclo ./gLib/gApp/mp8859.d ./gLib/gApp/mp8859.o ./gLib/gApp/mp8859.su

.PHONY: clean-gLib-2f-gApp

