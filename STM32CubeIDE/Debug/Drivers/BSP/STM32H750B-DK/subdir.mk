################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/PROGRAMIRANJE/Isystems/Isystems/Isystems_2021/Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_bus.c \
D:/PROGRAMIRANJE/Isystems/Isystems/Isystems_2021/Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_qspi.c \
D:/PROGRAMIRANJE/Isystems/Isystems/Isystems_2021/Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_sdram.c \
D:/PROGRAMIRANJE/Isystems/Isystems/Isystems_2021/Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_ts.c 

C_DEPS += \
./Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_bus.d \
./Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_qspi.d \
./Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_sdram.d \
./Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_ts.d 

OBJS += \
./Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_bus.o \
./Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_qspi.o \
./Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_sdram.o \
./Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_ts.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_bus.o: D:/PROGRAMIRANJE/Isystems/Isystems/Isystems_2021/Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_bus.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DSTM32H750xx -DUSE_HAL_DRIVER -DVECT_TAB_QSPI -DDEBUG -c -I../../Core/Inc -I../../Drivers/CMSIS/Include -I../../TouchGFX/generated/gui_generated/include -I../../TouchGFX/target -I../../TouchGFX/App -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/ST/touchgfx/framework/include -I../../TouchGFX/target/generated -I../../Drivers/BSP/STM32H750B-DK -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/BSP/Components/Common -I../../TouchGFX/gui/include -I../../TouchGFX/generated/texts/include -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../TouchGFX/generated/images/include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../TouchGFX/generated/fonts/include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_bus.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_qspi.o: D:/PROGRAMIRANJE/Isystems/Isystems/Isystems_2021/Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_qspi.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DSTM32H750xx -DUSE_HAL_DRIVER -DVECT_TAB_QSPI -DDEBUG -c -I../../Core/Inc -I../../Drivers/CMSIS/Include -I../../TouchGFX/generated/gui_generated/include -I../../TouchGFX/target -I../../TouchGFX/App -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/ST/touchgfx/framework/include -I../../TouchGFX/target/generated -I../../Drivers/BSP/STM32H750B-DK -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/BSP/Components/Common -I../../TouchGFX/gui/include -I../../TouchGFX/generated/texts/include -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../TouchGFX/generated/images/include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../TouchGFX/generated/fonts/include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_qspi.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_sdram.o: D:/PROGRAMIRANJE/Isystems/Isystems/Isystems_2021/Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_sdram.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DSTM32H750xx -DUSE_HAL_DRIVER -DVECT_TAB_QSPI -DDEBUG -c -I../../Core/Inc -I../../Drivers/CMSIS/Include -I../../TouchGFX/generated/gui_generated/include -I../../TouchGFX/target -I../../TouchGFX/App -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/ST/touchgfx/framework/include -I../../TouchGFX/target/generated -I../../Drivers/BSP/STM32H750B-DK -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/BSP/Components/Common -I../../TouchGFX/gui/include -I../../TouchGFX/generated/texts/include -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../TouchGFX/generated/images/include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../TouchGFX/generated/fonts/include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_sdram.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_ts.o: D:/PROGRAMIRANJE/Isystems/Isystems/Isystems_2021/Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_ts.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DSTM32H750xx -DUSE_HAL_DRIVER -DVECT_TAB_QSPI -DDEBUG -c -I../../Core/Inc -I../../Drivers/CMSIS/Include -I../../TouchGFX/generated/gui_generated/include -I../../TouchGFX/target -I../../TouchGFX/App -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/ST/touchgfx/framework/include -I../../TouchGFX/target/generated -I../../Drivers/BSP/STM32H750B-DK -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/BSP/Components/Common -I../../TouchGFX/gui/include -I../../TouchGFX/generated/texts/include -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../TouchGFX/generated/images/include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../TouchGFX/generated/fonts/include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/STM32H750B-DK/stm32h750b_discovery_ts.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

