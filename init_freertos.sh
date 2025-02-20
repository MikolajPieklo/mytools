#!/bin/bash

rm -rf FreeRTOS-Kernel

git clone --depth=1 --branch V11.1.0 --filter=blob:none --sparse https://github.com/FreeRTOS/FreeRTOS-Kernel.git
cd FreeRTOS-Kernel

git sparse-checkout init --cone
git sparse-checkout set --no-cone
git sparse-checkout set --skip-checks --negate CMakeLists.txt cmake/ include/CMakeLists.txt include/stdint.readme examples/template_configuration/readme.md portable/ThirdParty

git sparse-checkout set --skip-checks include portable/GCC/ARM_CM4_MPU portable/GCC/ARM_CM4F portable/GCC/ARM_CM3 examples/template_configuration portable/template \
   portable/MemMang portable/Common tasks.c queue.c list.c timers.c event_groups.c stream_buffer.c croutine.c

#rm -rf *.txt *.yaml *.url *.md *.yml
