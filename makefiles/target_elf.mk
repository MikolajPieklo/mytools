
.PHONY: ELF

ELF: $(OBJ_DIR)/$(NAME_STARTUP_FILE).o $(OBJ_CORE) $(OBJ_DRIVERS)
	@echo "$(ccblue)\nLinking... $(ccend)"
	$(SILENTMODE_FLAG) $(CC) $(LDFLAGS) $^ -o $(OUT_DIR)/target.elf