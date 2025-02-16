
.PHONY: HEX

HEX:
	@echo "$(ccblue)\nCreating hex file$(ccend)"
	$(CC_OBJCOPY) -O ihex $(OUT_DIR)/target.elf $(OUT_DIR)/target.hex

	@echo "$(ccblue)\nCreating bin file$(ccend)"
	$(CC_OBJCOPY) -O binary  $(OUT_DIR)/target.elf  $(OUT_DIR)/target.bin

	python tools/support/elf_update.py "out/target.elf"

	@echo "$(ccblue)\nGenerating list file$(ccend)"
	$(CC_OBJDUMP) -h -S  $(OUT_DIR)/target.elf > $(OUT_DIR)/target.list

	@echo "$(ccpurple)"
	arm-none-eabi-size $(OUT_DIR)/target.elf -A -x
	@echo "$(ccend)"