
.PHONY: make_sbl

make_sbl: $(OBJ_SBL) $(SBL_DIR)/sbl_startup_stm32f103c8tx.o
	@echo "$(ccblue)\nLinking sbl... $(ccend)"
	$(SILENTMODE_FLAG) $(CC) $(LDFLAGS) -T"tools/$(NAME_SBL_LINKER_SCRIPT).ld" $^ -o $(SBL_DIR)/sbl.elf