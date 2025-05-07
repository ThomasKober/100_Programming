################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../gLib/gCore/gBuf.c \
../gLib/gCore/gBuz.c \
../gLib/gCore/gCom.c \
../gLib/gCore/gComCallback.c \
../gLib/gCore/gCore.c \
../gLib/gCore/gInfo.c \
../gLib/gCore/gTemp.c 

OBJS += \
./gLib/gCore/gBuf.o \
./gLib/gCore/gBuz.o \
./gLib/gCore/gCom.o \
./gLib/gCore/gComCallback.o \
./gLib/gCore/gCore.o \
./gLib/gCore/gInfo.o \
./gLib/gCore/gTemp.o 

C_DEPS += \
./gLib/gCore/gBuf.d \
./gLib/gCore/gBuz.d \
./gLib/gCore/gCom.d \
./gLib/gCore/gComCallback.d \
./gLib/gCore/gCore.d \
./gLib/gCore/gInfo.d \
./gLib/gCore/gTemp.d 


# Each subdirectory must supply rules for building sources it contributes
gLib/gCore/%.o gLib/gCore/%.su gLib/gCore/%.cyclo: ../gLib/gCore/%.c gLib/gCore/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H562xx -c -I../Core/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../Drivers/CMSIS/Include -I../gLib/gCore -I../gLib/gApp -I../Middlewares/Third_Party/gtec_Driver/gCore/crypto/cipher -I../Middlewares/Third_Party/gtec_Driver/gCore/crypto -I../Middlewares/Third_Party/gtec_Driver/gCore/crypto/drbg -I../Middlewares/Third_Party/gtec_Driver/gCore/crypto/ecc -I../Middlewares/Third_Party/gtec_Driver/gCore/crypto/hash -I../Middlewares/Third_Party/gtec_Driver/gCore/crypto/mac -I../Middlewares/Third_Party/gtec_Driver/gCore/crypto/rsa -I../Middlewares/Third_Party/gtec_Driver/gCore/crypto/utils -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-gLib-2f-gCore

clean-gLib-2f-gCore:
	-$(RM) ./gLib/gCore/gBuf.cyclo ./gLib/gCore/gBuf.d ./gLib/gCore/gBuf.o ./gLib/gCore/gBuf.su ./gLib/gCore/gBuz.cyclo ./gLib/gCore/gBuz.d ./gLib/gCore/gBuz.o ./gLib/gCore/gBuz.su ./gLib/gCore/gCom.cyclo ./gLib/gCore/gCom.d ./gLib/gCore/gCom.o ./gLib/gCore/gCom.su ./gLib/gCore/gComCallback.cyclo ./gLib/gCore/gComCallback.d ./gLib/gCore/gComCallback.o ./gLib/gCore/gComCallback.su ./gLib/gCore/gCore.cyclo ./gLib/gCore/gCore.d ./gLib/gCore/gCore.o ./gLib/gCore/gCore.su ./gLib/gCore/gInfo.cyclo ./gLib/gCore/gInfo.d ./gLib/gCore/gInfo.o ./gLib/gCore/gInfo.su ./gLib/gCore/gTemp.cyclo ./gLib/gCore/gTemp.d ./gLib/gCore/gTemp.o ./gLib/gCore/gTemp.su

.PHONY: clean-gLib-2f-gCore

