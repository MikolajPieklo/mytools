
.PHONY: HEX_APP HEX_SBL HEX_COMBINED

HEX_APP:
	python tools/support/elf_update.py "out/app.elf"
	$(call generate_files_auto,out/app.elf)

ifeq ($(USE_SBL), yes)
HEX_SBL:
	$(call generate_files_auto,out/SBL/sbl.elf)
else
HEX_SBL:
endif

ifeq ($(USE_SBL),yes)
HEX_COMBINED:
	@echo "$(ccpurple)Combining SBL and APP into one HEX...$(ccend)"
	@if [ -f out/SBL/sbl.hex ] && [ -f out/app.hex ]; then \
		srec_cat out/SBL/sbl.hex -Intel \
		out/app.hex -Intel -offset 0x00 \
		-o out/combined.hex -Intel; \
	else \
		@echo "$(ccred)Error: SBL or APP HEX file not found for combining!$(ccend)" ; \
	fi
else
HEX_COMBINED:
endif

define generate_files_auto
	@echo "$(ccblue)\nProcessing $1$(ccend)"
	$(CC_OBJCOPY) -O ihex $1 $(basename $1).hex
	$(CC_OBJCOPY) -O binary $1 $(basename $1).bin
	$(CC_OBJDUMP) -h -S $1 > $(basename $1).list
	@echo "$(ccpurple)"
	arm-none-eabi-size $1 -A -x
	@echo "$(ccend)"
endef