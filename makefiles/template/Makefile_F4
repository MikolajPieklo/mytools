# Author: M Pieklo
# Date: 16.02.2025
# Project: STM32F401CCU6_HELLO_WORLD.
# License: Opensource
# Version: 1.0.2

include tools/makefiles/makefile_colors.mk
include tools/makefiles/makefile_info.mk
include tools/makefiles/makefile_dir.mk
include tools/makefiles/makefile_clib.mk
include tools/makefiles/makefile_common.mk

SILENTMODE := yes
USE_SBL := yes
USE_FREERTOS := yes
USE_DOUBLE_PRECISION := no
FREERTOS_HEAP := heap_1

NAME := $(OUT_DIR)/APP
NAME_STARTUP_FILE := startup_stm32f401ccux
NAME_SBL_STARTUP_FILE := sbl_startup_stm32f401ccux
NAME_APP_LINKER_SCRIPT := STM32F401CCUX_FLASH_APP
NAME_SBL_LINKER_SCRIPT := STM32F401CCUX_FLASH_SBL
SBL_SIZE_KB := 8
NAME_OPENOCD_CFG := stm32f4x
DEVICE := STM32F401xC
SW_FLAG := DEFAULT
MACH := cortex-m4
FLOAT_ABI := hard

include tools/makefiles/makefile_flags.mk

INC := \
	-ICore/MAIN/inc/ \
	-Itools/Reuse/inc \
	-IDrivers/STM32F4xx_HAL_Driver/inc/ \
	-IDrivers/CMSIS/Device/ST/STM32F4xx/Include/ \
	-IDrivers/CMSIS/Include/ \
	-Itools/Reuse/CC1101/inc/ \
	-Itools/Reuse/LCD12864/inc/ \
	-Itools/Reuse/Lora/inc \
	-Itools/Reuse/NRF24L01/inc/ \
	-Itools/Reuse/SH1106/inc/ \
	-Itools/Reuse/SI4432/inc/ \
	-Itools/Reuse/WS25Qxx/inc/ \
	-Itools/freertos/include/ \
	-Itools/freertos/portable/GCC/ARM_CM4F/

SRC_CORE_DIRS := Core/MAIN/src
SRC_DRIVERS_DIR := Drivers/STM32F4xx_HAL_Driver/src
SRC_SBL := tools/SBL/src

########################################################################################################################

.PHONY: all release sbl app

all: check_flags DIR make_app make_sbl HEX_APP HEX_SBL HEX_COMBINED
sbl: check_flags DIR make_sbl HEX_SBL
app: check_flags DIR make_app HEX_APP
release : all

include tools/makefiles/makefile_dependencies.mk

include tools/makefiles/target_check_flags.mk
include tools/makefiles/target_chip.mk
include tools/makefiles/target_clean.mk
include tools/makefiles/target_dir.mk
include tools/makefiles/target_doc.mk
include tools/makefiles/make_app.mk
include tools/makefiles/make_sbl.mk
include tools/makefiles/target_hex.mk