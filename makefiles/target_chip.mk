
.PHONY: load load_app load_sbl restart reset erase connect verify test dump

OPENOCD_IFACE  := /usr/share/openocd/scripts/interface/stlink.cfg
OPENOCD_TARGET := /usr/share/openocd/scripts/target/$(NAME_OPENOCD_CFG).cfg

OPENOCD    := openocd -f $(OPENOCD_IFACE) -f $(OPENOCD_TARGET)
IMAGE_DUMP := out/flash_dump.bin

ifeq ($(USE_SBL), yes)
IMAGE           := out/combined.hex
IMAGE_DUMP_ADDR := $(shell printf "0x%08x" $$((0x08000000 + $(SBL_SIZE_KB)*1024)))
else
IMAGE           := out/app.hex
IMAGE_DUMP_ADDR := 0x08000000
endif

load:
	./tools/support/flash_stlink.sh "$(NAME_OPENOCD_CFG)" "$(IMAGE)"

load_app:
	./tools/support/flash_stlink.sh "$(NAME_OPENOCD_CFG)" "out/app.hex"

load_sbl:
	./tools/support/flash_stlink.sh "$(NAME_OPENOCD_CFG)" "out/SBL/sbl.hex"

restart:
	$(OPENOCD) -c "init" -c "reset" -c "exit"

reset: restart

erase:
	$(OPENOCD) -c "init" -c "halt" -c "flash erase_sector 0 0 last" -c "reset" -c "exit"

connect:
	$(OPENOCD) -c "init" -c "halt"

verify:
	$(OPENOCD) -c "init" -c "halt" -c "flash verify_image $(IMAGE)" -c "reset" -c "exit"

test:
	$(OPENOCD) -c "init" -c "targets" -c "dap info" -c "flash info 0" -c "shutdown"

dump:
	st-flash read $(IMAGE_DUMP) $(IMAGE_DUMP_ADDR) 65536
#	$(OPENOCD) -c "init" -c "halt" -c "dump_image $(IMAGE_DUMP) $(IMAGE_DUMP_ADDR) 0x90E0" -c "reset" -c "exit"

#serial number st-link
#53FF6C064965525327141187 - purple
#363B15157116303030303032 - gold