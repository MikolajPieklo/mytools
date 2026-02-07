OUT_DIR          := out
OBJ_DIR          := $(OUT_DIR)/obj
BIN_DIR          := $(OUT_DIR)/bin
LIB_DIR          := $(OUT_DIR)/lib
DRIVER_DIR       := $(OUT_DIR)/Drivers
RTOS_DIR         := $(OUT_DIR)/RTOS
RTOS_MODULES_DIR := $(OUT_DIR)/RTOS_MODULES
TUSB_DIR         := $(OUT_DIR)/tusb
REUSE_DIR        := $(OUT_DIR)/Reuse
SBL_DIR          := $(OUT_DIR)/SBL
DOC_DIR          := $(OUT_DIR)/doc

BUILD_FLAGS_FILE      := $(OUT_DIR)/.build_flags
TEMP_BUILD_FLAGS_FILE := $(OUT_DIR)/.temp_build_flags

.DIR:
	@if [ ! -e $(OUT_DIR) ]; then mkdir $(OUT_DIR); echo "$(CFLAGS)" > $(BUILD_FLAGS_FILE); fi
	@if [ ! -e $(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi
	@if [ ! -e $(DRIVER_DIR) ]; then mkdir $(DRIVER_DIR); fi
	@if [ ! -e $(REUSE_DIR) ]; then mkdir $(REUSE_DIR); fi
	@if [ ! -e $(DOC_DIR) ]; then mkdir $(DOC_DIR); fi
	@if [ "$(USE_SBL)" = "yes" ]; then if [ ! -d "$(SBL_DIR)" ]; then mkdir "$(SBL_DIR)"; fi fi
	@if [ "$(USE_RTOS)" = "yes" ]; then if [ ! -d "$(RTOS_DIR)" ]; then mkdir "$(RTOS_DIR)"; fi fi
	@if [ "$(USE_RTOS)" = "yes" ] && [ ! -d "$(RTOS_MODULES_DIR)" ]; then mkdir "$(RTOS_MODULES_DIR)"; fi