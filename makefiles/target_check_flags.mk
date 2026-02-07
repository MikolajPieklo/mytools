.check_flags:

	@if [ -n "$(USE_SBL)" ]; then \
		echo "USE_SBL is defined: $(USE_SBL)"; \
	else \
		echo "USE_SBL is not defined"; \
		USE_SBL ?= no; \
	fi

	@if [ -n "$(USE_RTOS)" ]; then \
		echo "USE_RTOS is defined: $(USE_RTOS)"; \
	else \
		echo "USE_RTOS is not defined"; \
		USE_RTOS ?= no; \
	fi

	@if [ -e $(OUT_DIR) ]; then \
		echo "$(CFLAGS)" > $(TEMP_BUILD_FLAGS_FILE); \
		if [ ! -f $(BUILD_FLAGS_FILE) ] || ! cmp -s $(TEMP_BUILD_FLAGS_FILE) $(BUILD_FLAGS_FILE); then \
			echo "BUILD FLAGS HAS BEEN CHANGED! REBUILD..."; \
			rm -rf $(OUT_DIR); \
		else \
			rm -f $(TEMP_BUILD_FLAGS_FILE); \
		fi \
	fi