
SRC_CORE_DIR_WITHOUT_PREFIX := $(foreach dir, $(SRC_CORE_DIRS), $(patsubst Core/%, %, $(dir)))
SRC_CORE := $(foreach dir, $(SRC_CORE_DIRS), $(wildcard $(dir)/*.c))
LIST_SRC_DRIVERS := $(wildcard $(SRC_DRIVERS_DIR)/*.c)

PATH_REUSE_BASE := tools/Reuse
LIST_SRC_REUSE := $(shell find $(PATH_REUSE_BASE) -type f -name "*.c")

PATH_SBL_BASE := tools/bootloader
ifeq ($(DEVICE),STM32F103xB)
	PATH_SBL_MACH_DIR := $(PATH_SBL_BASE)/stm32f103c8tx
	PATH_SBL_DIR := $(PATH_SBL_BASE)/src $(PATH_SBL_MACH_DIR)
else ifeq ($(DEVICE),STM32F401xC)
	PATH_SBL_MACH_DIR := $(PATH_SBL_BASE)/stm32f401ccux
	PATH_SBL_DIR := $(PATH_SBL_BASE)/src $(PATH_SBL_MACH_DIR)
else
	PATH_SBL_MACH_DIR :=
	PATH_SBL_DIR :=
endif

LIST_SRC_SBL := $(foreach dir,$(PATH_SBL_DIR),$(shell find $(dir) -type f -name "*.c"))

ifdef USE_FREERTOS
	ifeq ($(USE_FREERTOS), yes)
		PATH_RTOS_DIR := tools/freertos
		SRC_RTOS_DIR_PORT := tools/freertos/portable/GCC/ARM_CM4F
		SRC_RTOS_DIR_HEAP := tools/freertos/portable/MemMang
		SRC_RTOS := $(foreach dir, $(PATH_RTOS_DIR), $(wildcard $(dir)/*.c))
	endif
endif

OBJ_CORE := $(patsubst Core/%.c, $(OBJ_DIR)/%.o, $(SRC_CORE))
OBJ_DRIVERS := $(LIST_SRC_DRIVERS:$(SRC_DRIVERS_DIR)/%.c=$(DRIVER_DIR)/%.o)
OBJ_REUSE := $(patsubst $(PATH_REUSE_BASE)/%.c,$(REUSE_DIR)/%.o,$(LIST_SRC_REUSE))
OBJ_SBL := $(patsubst $(PATH_SBL_BASE)/%.c,$(SBL_DIR)/%.o,$(LIST_SRC_SBL))

ifdef USE_FREERTOS
	ifeq ($(USE_FREERTOS), yes)
		OBJ_RTOS := $(patsubst tools/freertos/%.c, $(RTOS_DIR)/%.o, $(SRC_RTOS))
		OBJ_RTOS += out/RTOS/port.o
		ifeq ($(FREERTOS_HEAP), heap_1)
			OBJ_RTOS += out/RTOS/heap_1.o
		else ifeq ($(FREERTOS_HEAP), heap_2)
			OBJ_RTOS += out/RTOS/heap_2.o
		else ifeq ($(FREERTOS_HEAP), heap_3)
			OBJ_RTOS += out/RTOS/heap_3.o
		else ifeq ($(FREERTOS_HEAP), heap_4)
			OBJ_RTOS += out/RTOS/heap_4.o
		else ifeq ($(FREERTOS_HEAP), heap_5)
			OBJ_RTOS += out/RTOS/heap_5.o
		else
			$(error FREERTOS_HEAP is not define!)
		endif
	endif
endif

$(OBJ_DIR)/%.o: Core/%.c
	@echo "Compiling $< -> $@"
	@mkdir -p $(dir $@)
	$(SILENTMODE_FLAG) $(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INC) $< -o $@

$(DRIVER_DIR)/%.o: $(SRC_DRIVERS_DIR)/%.c
	@echo "Compiling $< -> $@"
	$(SILENTMODE_FLAG) $(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INC) $< -o $@

$(REUSE_DIR)/%.o: $(PATH_REUSE_BASE)/%.c
	@echo "Compiling $< -> $@"
	@mkdir -p $(dir $@)
	$(SILENTMODE_FLAG) $(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INC) $< -o $@

$(SBL_DIR)/%.o: $(PATH_SBL_BASE)/%.c
	@echo "Compiling $< -> $@"
	@mkdir -p $(dir $@)
	$(SILENTMODE_FLAG) $(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INC) $< -o $@

ifdef USE_FREERTOS
ifeq ($(USE_FREERTOS), yes)
$(RTOS_DIR)/%.o: $(PATH_RTOS_DIR)/%.c
	@echo "Compiling $< -> $@"
	$(SILENTMODE_FLAG) $(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INC) $< -o $@

$(RTOS_DIR)/%.o: $(SRC_RTOS_DIR_PORT)/%.c
	@echo "Compiling $< -> $@"
	$(SILENTMODE_FLAG) $(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INC) $< -o $@

$(RTOS_DIR)/%.o: $(SRC_RTOS_DIR_HEAP)/%.c
	@echo "Compiling $< -> $@"
	$(SILENTMODE_FLAG) $(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INC) $< -o $@
endif
endif

# $^ dependency $@ target
$(OBJ_DIR)/$(NAME_STARTUP_FILE).o: Core/Startup/$(NAME_STARTUP_FILE).s
	@echo "Compiling $< -> $@"
	$(SILENTMODE_FLAG) $(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $< -o $@

$(SBL_DIR)/$(NAME_SBL_STARTUP_FILE).o: $(PATH_SBL_MACH_DIR)/$(NAME_SBL_STARTUP_FILE).s
	@echo "Compiling $< -> $@"
	$(SILENTMODE_FLAG) $(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $< -o $@

-include $(OBJ_CORE:.o=.d)
-include $(OBJ_DRIVERS:.o=.d)
-include $(OBJ_REUSE:.o=.d)
-include $(OBJ_RTOS:.o=.d)
-include $(OBJ_SBL:.o=.d)