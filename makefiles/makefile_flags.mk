
MAP  := -Wl,-Map=$(NAME).map  # Create map file
GC   := -Wl,--gc-sections     # Link for code size

CFLAGS := \
	-c \
	-mcpu=$(MACH) \
	-mthumb \
	-mfloat-abi=$(FLOAT_ABI) \
	-std=gnu11 \
	-O$(OPTIMIZATION) \
	-D$(DEVICE) \
	-D$(SW_FLAG) \
	-DSBL_SIZE_KB=$(SBL_SIZE_KB) \
	$(USE_NANO) \
	-Wall \
	-Wextra \
	-ffunction-sections \
	-fdata-sections \
	-fstack-usage \
	-fno-common \
	-MMD \
	-Wfatal-errors \
	-Werror=implicit \
	-fdiagnostics-color=always \
	-Wnull-dereference \
	-Wuninitialized \
	-Wreturn-type \
	-Wredundant-decls \
	-Wunused \
	-Wundef

CFLAGS_SBL = $(CFLAGS) -DSBL_BUILD=1
CFLAGS_SBL := $(subst -DUSED_RTOS=1, ,$(CFLAGS_SBL))
CFLAGS += -DAPP_BUILD=1

LDFLAGS := \
	-mcpu=$(MACH) \
	-mthumb \
	-mfloat-abi=$(FLOAT_ABI) \
	$(MAP) \
	$(GC) \
	-static \
	$(USE_NANO) \
	-fdiagnostics-color=always \
	-Wl,--start-group -lc -lm -Wl,--end-group \
	-Wl,--print-memory-usage \
	-Wl,--cref

ifeq ($(USE_SBL), no)
	SBL_SIZE_KB := 0
endif
	LDFLAGS += -Wl,-defsym,SBL_SIZE_KB=$(SBL_SIZE_KB)

ifeq ($(MACH), cortex-m4)
	CFLAGS += -mfpu=fpv4-sp-d16
	LDFLAGS += -mfpu=fpv4-sp-d16

	CONST := -DUSE_FULL_LL_DRIVER -DHSE_VALUE=25000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 \
		-DLSE_VALUE=32768 -DHSI_VALUE=16000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DUSE_FULL_ASSERT=1U -DPREFETCH_ENABLE=1 \
		-DINSTRUCTION_CACHE_ENABLE=1 -DDATA_CACHE_ENABLE=1 -DEXTERNAL_CLOCK_VALUE=12288000 $(CC_COMMON_MACRO)
else ifeq ($(MACH) ,cortex-m3)
	CONST := -DUSE_FULL_LL_DRIVER -DHSE_VALUE=8000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 \
		-DLSE_VALUE=32768 -DHSI_VALUE=8000000 -DLSI_VALUE=40000 -DVDD_VALUE=3300 -DUSE_FULL_ASSERT -DPREFETCH_ENABLE=1 \
		$(CC_COMMON_MACRO)
endif

ifdef (USE_DOUBLE_PRECISION)
	ifeq ($(USE_DOUBLE_PRECISION), no)
		CFLAGS += -fsingle-precision-constant
	endif
endif

SILENTMODE_FLAG :=
ifeq ($(SILENTMODE), yes)
SILENTMODE_FLAG := @
endif

ifeq ($(USE_RTOS), yes)
	CFLAGS += -DUSED_RTOS=1
endif

ifeq ($(USE_SBL), yes)
	CFLAGS += -DUSED_SBL=1
endif