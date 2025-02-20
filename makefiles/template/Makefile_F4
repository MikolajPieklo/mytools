# Author: M Pieklo
# Date: 16.02.2025
# Project: STM32F401CCU6_HELLO_WORLD.
# License: Opensource

include tools/makefiles/makefile_colors.mk
include tools/makefiles/makefile_info.mk
include tools/makefiles/makefile_dir.mk
include tools/makefiles/makefile_clib.mk
include tools/makefiles/makefile_common.mk

SILENTMODE := yes
USE_FREERTOS := yes
FREERTOS_HEAP := heap_1

NAME := $(OUT_DIR)/TARGET
NAME_STARTUP_FILE := startup_stm32f401ccux
NAME_LINKER_SCRIPT := STM32F401CCUX_FLASH
NAME_OPENOCD_CFG := stm32f4x
DEVICE := STM32F401xC
SW_FLAG := DEFAULT
MACH := cortex-m4
FLOAT_ABI := hard
MAP  := -Wl,-Map=$(NAME).map  # Create map file
GC   := -Wl,--gc-sections     # Link for code size

include tools/makefiles/makefile_flags.mk

INC := \
	-ICore/MAIN/inc/ \
	-Itools/Reuse/inc/ \
	-IDrivers/STM32F4xx_HAL_Driver/inc/ \
	-IDrivers/CMSIS/Device/ST/STM32F4xx/Include/ \
	-IDrivers/CMSIS/Include/ \
	-Itools/FreeRTOS-Kernel/include/ \
	-Itools/FreeRTOS-Kernel/portable/GCC/ARM_CM4F/

SRC_CORE_DIRS := Core/MAIN/src
SRC_DRIVERS_DIR := Drivers/STM32F4xx_HAL_Driver/src

########################################################################################################################

.PHONY: all release

all: check_flags DIR ELF HEX

release : all

include tools/makefiles/makefile_dependencies.mk

include tools/makefiles/target_check_flags.mk
include tools/makefiles/target_chip.mk
include tools/makefiles/target_clean.mk
include tools/makefiles/target_dir.mk
include tools/makefiles/target_doc.mk
include tools/makefiles/target_elf.mk
include tools/makefiles/target_hex.mk