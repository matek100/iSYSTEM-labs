################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Application/Startup/startup_stm32h750xbhx.s 

S_DEPS += \
./Application/Startup/startup_stm32h750xbhx.d 

OBJS += \
./Application/Startup/startup_stm32h750xbhx.o 


# Each subdirectory must supply rules for building sources it contributes
Application/Startup/startup_stm32h750xbhx.o: ../Application/Startup/startup_stm32h750xbhx.s
	arm-none-eabi-gcc -mcpu=cortex-m7 -g3 -c -x assembler-with-cpp -MMD -MP -MF"Application/Startup/startup_stm32h750xbhx.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

