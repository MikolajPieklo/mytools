
.PHONY: load load_app load_sbl restart reset erase

ifeq ($(USE_SBL), yes)
load:
	./tools/support/flash_stlink.sh "$(NAME_OPENOCD_CFG)" "out/combined.hex"
else
load:
	./tools/support/flash_stlink.sh "$(NAME_OPENOCD_CFG)" "out/app.hex"
endif

load_app:
	./tools/support/flash_stlink.sh "$(NAME_OPENOCD_CFG)" "out/app.hex"

load_sbl:
	./tools/support/flash_stlink.sh "$(NAME_OPENOCD_CFG)" "out/SBL/sbl.hex"

restart:
	openocd -f /usr/share/openocd/scripts/interface/stlink.cfg \
		-f /usr/share/openocd/scripts/target/$(NAME_OPENOCD_CFG).cfg \
		-c "init" \
		-c "reset" \
		-c "exit"

reset: restart

erase:
	openocd -f /usr/share/openocd/scripts/interface/stlink.cfg \
		-f /usr/share/openocd/scripts/target/$(NAME_OPENOCD_CFG).cfg \
		-c "init" \
		-c "halt" \
		-c "flash erase_sector 0 0 last" \
		-c "reset" \
		-c "exit"

#serial number st-link
#53FF6C064965525327141187 - purple
#363B15157116303030303032 - gold