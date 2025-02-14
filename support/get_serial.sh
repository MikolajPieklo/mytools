#!/bin/bash

serials=$(st-info --serial)

if [[ -z "$serials" ]]; then
    echo "No connected ST-Link."
    exit 1
fi

echo "List of ST-Links: "
select serial in $serials; do
    if [[ -n "$serial" ]]; then
        break
    else
        echo "Error. Try again."
    fi
done

#	openocd -f /usr/local/share/openocd/scripts/interface/st-link.cfg \
#		-c "adapter serial 363B15157116303030303032" \
#		-f /usr/local/share/openocd/scripts/target/stm32f1x.cfg \
#		-c "program $(OUT_DIR)/target.elf verify reset exit"

# openocd -f /usr/local/share/openocd/scripts/interface/st-link.cfg \
# 	-c "hla_serial $$st_serial_nr" \
# 	-f /usr/local/share/openocd/scripts/target/stm32f1x.cfg \
# 	-c "program $(OUT_DIR)/target.elf verify reset exit";

#load:
#	@st_serial_nr="363B15157116303030303032";
#	@st_serial_nr=$$(./support/get_serial.sh);
#	@st_serial_nr="ket $$(./support/get_serial.sh)"; \
#	echo "dupa $$(st_serial_nr)"