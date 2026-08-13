################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rzn/fsp/src/bsp/mcu/all/bsp_cache.c \
../rzn/fsp/src/bsp/mcu/all/bsp_clocks.c \
../rzn/fsp/src/bsp/mcu/all/bsp_common.c \
../rzn/fsp/src/bsp/mcu/all/bsp_delay.c \
../rzn/fsp/src/bsp/mcu/all/bsp_io.c \
../rzn/fsp/src/bsp/mcu/all/bsp_irq.c \
../rzn/fsp/src/bsp/mcu/all/bsp_register_protection.c \
../rzn/fsp/src/bsp/mcu/all/bsp_reset.c \
../rzn/fsp/src/bsp/mcu/all/bsp_sbrk.c 

LST += \
bsp_cache.lst \
bsp_clocks.lst \
bsp_common.lst \
bsp_delay.lst \
bsp_io.lst \
bsp_irq.lst \
bsp_register_protection.lst \
bsp_reset.lst \
bsp_sbrk.lst 

C_DEPS += \
./rzn/fsp/src/bsp/mcu/all/bsp_cache.d \
./rzn/fsp/src/bsp/mcu/all/bsp_clocks.d \
./rzn/fsp/src/bsp/mcu/all/bsp_common.d \
./rzn/fsp/src/bsp/mcu/all/bsp_delay.d \
./rzn/fsp/src/bsp/mcu/all/bsp_io.d \
./rzn/fsp/src/bsp/mcu/all/bsp_irq.d \
./rzn/fsp/src/bsp/mcu/all/bsp_register_protection.d \
./rzn/fsp/src/bsp/mcu/all/bsp_reset.d \
./rzn/fsp/src/bsp/mcu/all/bsp_sbrk.d 

OBJS += \
./rzn/fsp/src/bsp/mcu/all/bsp_cache.o \
./rzn/fsp/src/bsp/mcu/all/bsp_clocks.o \
./rzn/fsp/src/bsp/mcu/all/bsp_common.o \
./rzn/fsp/src/bsp/mcu/all/bsp_delay.o \
./rzn/fsp/src/bsp/mcu/all/bsp_io.o \
./rzn/fsp/src/bsp/mcu/all/bsp_irq.o \
./rzn/fsp/src/bsp/mcu/all/bsp_register_protection.o \
./rzn/fsp/src/bsp/mcu/all/bsp_reset.o \
./rzn/fsp/src/bsp/mcu/all/bsp_sbrk.o 

MAP += \
rzn2l_xspi_boot_loader.map 


# Each subdirectory must supply rules for building sources it contributes
rzn/fsp/src/bsp/mcu/all/%.o: ../rzn/fsp/src/bsp/mcu/all/%.c
	@echo 'Building file: $<'
	$(file > $@.in,-mcpu=cortex-r52 -mthumb -mfloat-abi=hard -mfpu=neon-fp-armv8 -fdiagnostics-parseable-fixits -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -Wnull-dereference -g -gdwarf-4 -D_RENESAS_RZN_ -D_RZN_CORE=CR52_0 -D_RZN_ORDINAL=1 -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\src" -I"." -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn\\fsp\\inc" -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn\\fsp\\inc\\api" -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn\\fsp\\inc\\instances" -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn\\fsp\\src\\bsp\\mcu\\all\\cr" -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn\\arm\\CMSIS_5\\CMSIS\\Core_R\\Include" -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn_gen" -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn_cfg\\fsp_cfg\\bsp" -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn_cfg\\fsp_cfg" -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn_cfg\\driver" -std=c99 -Wno-format-truncation -Wno-stringop-overflow --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<")
	@arm-none-eabi-gcc @"$@.in"

