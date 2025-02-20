CFLAGS := \
	-c \
	-mcpu=$(MACH) \
	-mthumb \
	-mfloat-abi=$(FLOAT_ABI) \
	-std=gnu11 \
	-O$(OPTIMIZATION) \
	-D$(DEVICE) \
	-D$(SW_FLAG) \
	$(USE_NANO) \
	-Wall \
	-Wextra \
	-ffunction-sections \
	-fdata-sections \
	-fstack-usage \
	-MMD \
	-Wfatal-errors \
	-Werror=implicit \
	-fdiagnostics-color=always

LDFLAGS := \
	-mcpu=$(MACH) \
	-mthumb \
	-mfloat-abi=$(FLOAT_ABI) \
	-T"tools/$(NAME_LINKER_SCRIPT).ld" \
	$(MAP) \
	$(GC) \
	-static \
	$(USE_NANO) \
	-fdiagnostics-color=always \
	-Wl,--start-group -lc -lm -Wl,--end-group

ifeq ($(MACH), cortex-m4)

CFLAGS += -mfpu=fpv4-sp-d16
LD += -mfpu=fpv4-sp-d16

CONST := -DUSE_FULL_LL_DRIVER -DHSE_VALUE=25000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 \
	-DLSE_VALUE=32768 -DHSI_VALUE=16000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DUSE_FULL_ASSERT=1U -DPREFETCH_ENABLE=1 \
	-DINSTRUCTION_CACHE_ENABLE=1 -DDATA_CACHE_ENABLE=1 -DEXTERNAL_CLOCK_VALUE=12288000 $(CC_COMMON_MACRO)

else ifeq ($(MACH) ,cortex-m3)

CONST := -DUSE_FULL_LL_DRIVER -DHSE_VALUE=8000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 \
	-DLSE_VALUE=32768 -DHSI_VALUE=8000000 -DLSI_VALUE=40000 -DVDD_VALUE=3300 -DUSE_FULL_ASSERT -DPREFETCH_ENABLE=1 \
	$(CC_COMMON_MACRO)
endif

ifeq ($(USE_FREERTOS), yes)
CFLAGS += -DUSE_RTOS
endif