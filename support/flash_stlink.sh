#!/bin/bash

serials=$(st-info --serial)

if [[ -z "$serials" ]]; then
   echo "No connected ST-Link."
   exit 1
fi

echo "$serials"
count=$(echo "$serials" | wc -l)

echo "$count"

if [[ "$count" -eq 1 ]]; then
   serial="$serials"
else
   echo "List of ST-Links: "
   select serial in $serials; do
      if [[ -n "$serial" ]]; then
         break
      else
         echo "Error. Try again."
      fi
   done
fi

openocd -f /usr/share/openocd/scripts/interface/stlink.cfg \
        -c "hla_serial $serial" \
        -f /usr/share/openocd/scripts/target/$1.cfg \
        -c "program out/target.elf verify reset exit"


# openocd -f interface.cfg -f target.cfg -c "program firmware.bin 0x08004000 verify reset exit"