
.PHONY: HEX

HEX:
	python tools/support/elf_update.py "out/app.elf"

	@echo "$(ccblue)\nCreating hex file$(ccend)"
	$(CC_OBJCOPY) -O ihex $(OUT_DIR)/app.elf $(OUT_DIR)/app.hex

	@echo "$(ccblue)\nCreating bin file$(ccend)"
	$(CC_OBJCOPY) -O binary  $(OUT_DIR)/app.elf  $(OUT_DIR)/app.bin

	@echo "$(ccblue)\nGenerating list file$(ccend)"
	$(CC_OBJDUMP) -h -S  $(OUT_DIR)/app.elf > $(OUT_DIR)/app.list

	@echo "$(ccpurple)"
	arm-none-eabi-size $(OUT_DIR)/app.elf -A -x
	@echo "$(ccend)"