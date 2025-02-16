
.PHONY: check_flags

check_flags:
	@if [ -e $(OUT_DIR) ]; then \
		echo "$(CFLAGS)" > $(TEMP_BUILD_FLAGS_FILE); \
		if [ ! -f $(BUILD_FLAGS_FILE) ] || ! cmp -s $(TEMP_BUILD_FLAGS_FILE) $(BUILD_FLAGS_FILE); then \
			echo "BUILD FLAGS HAS BEEN CHANGED! REBUILD..."; \
			rm -rf $(OUT_DIR); \
		else \
			rm -f $(TEMP_BUILD_FLAGS_FILE); \
		fi \
	fi