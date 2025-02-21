# Author: M Pieklo
# Date: 06.11.2024
# Project: STM32F103C8T6_HELLO_WORLD.
# License: Opensource

include tools/makefiles/makefile_colors.mk
include tools/makefiles/makefile_info.mk
include tools/makefiles/makefile_dir.mk
include tools/makefiles/makefile_clib.mk
include tools/makefiles/makefile_common.mk

SILENTMODE := yes
USE_FREERTOS := no
FREERTOS_HEAP := heap_1

NAME := $(OUT_DIR)/TARGET
NAME_STARTUP_FILE := startup_stm32f103c8tx
NAME_LINKER_SCRIPT := STM32F103C8TX_FLASH
NAME_OPENOCD_CFG := stm32f1x
DEVICE := STM32F103xB
SW_FLAG := LORA_E32_RX
MACH := cortex-m3
FLOAT_ABI := soft
MAP  := -Wl,-Map=$(NAME).map  # Create map file
GC   := -Wl,--gc-sections     # Link for code size

include tools/makefiles/makefile_flags.mk

INC := \
	-ICore/MAIN/inc/ \
	-ICore/Flash/inc \
	-ICore/LCD12864/inc \
	-ICore/CC1101/inc \
	-ICore/Lora/inc \
	-ICore/SH1106/inc \
	-ICore/SI4432/inc \
	-ICore/NRF24L01/inc \
	-Itools/Reuse/inc \
	-IDrivers/STM32F1xx_HAL_Driver/inc/ \
	-IDrivers/CMSIS/Device/ST/STM32F1xx/Include/ \
	-IDrivers/CMSIS/Include/

SRC_CORE_DIRS := Core/MAIN/src Core/Flash/src Core/LCD12864/src Core/CC1101/src Core/Lora/src Core/SH1106/src \
					Core/SI4432/src Core/NRF24L01/src

SRC_DRIVERS_DIR := Drivers/STM32F1xx_HAL_Driver/src

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
