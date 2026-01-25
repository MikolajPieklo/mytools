
.PHONY: DIR

DIR:
	@if [ ! -e $(OUT_DIR) ]; then mkdir $(OUT_DIR); echo "$(CFLAGS)" > $(BUILD_FLAGS_FILE); fi
	@if [ ! -e $(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi
	@if [ ! -e $(DRIVER_DIR) ]; then mkdir $(DRIVER_DIR); fi
	@if [ ! -e $(REUSE_DIR) ]; then mkdir $(REUSE_DIR); fi
	@if [ "$(USE_SBL)" = "yes" ]; then if [ ! -d "$(SBL_DIR)" ]; then mkdir "$(SBL_DIR)"; fi fi
	@if [ "$(USE_RTOS)" = "yes" ]; then if [ ! -d "$(RTOS_DIR)" ]; then mkdir "$(RTOS_DIR)"; fi fi