
.PHONY: DIR

DIR:
	@if [ ! -e $(OUT_DIR) ]; then mkdir $(OUT_DIR); echo "$(CFLAGS)" > $(BUILD_FLAGS_FILE); fi
	@if [ ! -e $(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi
	@if [ ! -e $(DRIVER_DIR) ]; then mkdir $(DRIVER_DIR); fi
	@if [ ! -e $(REUSE_DIR) ]; then mkdir $(REUSE_DIR); fi
	@$(foreach dir,$(SRC_CORE_DIR_WITHOUT_PREFIX), \
		mkdir -p $(OBJ_DIR)/$(dir); \
	)