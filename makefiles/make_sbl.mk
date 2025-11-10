
.PHONY: make_sbl

ifeq ($(USE_SBL), yes)
make_sbl: $(OBJ_SBL) $(SBL_DIR)/sbl_startup_stm32f103c8tx.o $(OBJ_DRIVERS) $(OBJ_REUSE)
	@echo "$(ccblue)\nLinking sbl... $(ccend)"
	@echo "$(cccyan)"
	$(SILENTMODE_FLAG) $(CC) $(LDFLAGS) -T"tools/$(NAME_SBL_LINKER_SCRIPT).ld" $^ -o $(SBL_DIR)/sbl.elf
	@echo "$(ccend)"
else
make_sbl:
endif