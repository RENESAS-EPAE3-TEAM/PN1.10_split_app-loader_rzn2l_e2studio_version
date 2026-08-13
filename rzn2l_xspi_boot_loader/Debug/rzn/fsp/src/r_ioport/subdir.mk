################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rzn/fsp/src/r_ioport/r_ioport.c 

LST += \
r_ioport.lst 

C_DEPS += \
./rzn/fsp/src/r_ioport/r_ioport.d 

OBJS += \
./rzn/fsp/src/r_ioport/r_ioport.o 

MAP += \
rzn2l_xspi_boot_loader.map 


# Each subdirectory must supply rules for building sources it contributes
rzn/fsp/src/r_ioport/%.o: ../rzn/fsp/src/r_ioport/%.c
	@echo 'Building file: $<'
	$(file > $@.in,-mcpu=cortex-r52 -mthumb -mfloat-abi=hard -mfpu=neon-fp-armv8 -fdiagnostics-parseable-fixits -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -Wnull-dereference -g -gdwarf-4 -D_RENESAS_RZN_ -D_RZN_CORE=CR52_0 -D_RZN_ORDINAL=1 -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\src" -I"." -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn\\fsp\\inc" -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn\\fsp\\inc\\api" -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn\\fsp\\inc\\instances" -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn\\fsp\\src\\bsp\\mcu\\all\\cr" -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn\\arm\\CMSIS_5\\CMSIS\\Core_R\\Include" -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn_gen" -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn_cfg\\fsp_cfg\\bsp" -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn_cfg\\fsp_cfg" -I"C:\\Users\\Administrator\\Desktop\\Renesas_PROFINET_IRT_DEVKIT_V1.10.0\\gcc_project\\rzn2l_xspi_boot_loader\\rzn_cfg\\driver" -std=c99 -Wno-format-truncation -Wno-stringop-overflow --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<")
	@arm-none-eabi-gcc @"$@.in"

