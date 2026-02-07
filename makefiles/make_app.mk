
.PHONY: make_app

make_app: $(OBJ_CORE) $(OBJ_DRIVERS) $(OBJ_REUSE) $(OBJ_RTOS) $(OBJ_RTOS_MODULES) $(OBJ_DIR)/$(NAME_STARTUP_FILE).o
	@echo "$(ccblue)\nLinking app... $(ccend)"
	@echo "$(cccyan)"
	$(SILENTMODE_FLAG) $(CC) $(LDFLAGS) -T"tools/$(NAME_APP_LINKER_SCRIPT).ld" $^ -o $(OUT_DIR)/app.elf
	@echo "$(ccend)"