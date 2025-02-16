
SRC_CORE_DIR_WITHOUT_PREFIX := $(foreach dir, $(SRC_CORE_DIRS), $(patsubst Core/%, %, $(dir)))
SRC_CORE := $(foreach dir, $(SRC_CORE_DIRS), $(wildcard $(dir)/*.c))
SRC_DRIVERS := $(wildcard $(SRC_DRIVERS_DIR)/*.c)

OBJ_CORE := $(patsubst Core/%.c, $(OBJ_DIR)/%.o, $(SRC_CORE))
OBJ_DRIVERS := $(SRC_DRIVERS:$(SRC_DRIVERS_DIR)/%.c=$(DRIVER_DIR)/%.o)

$(OBJ_DIR)/%.o: Core/%.c
	@echo "Compiling $<..."
	$(SILENTMODE_FLAG) $(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INC) $< -o $@

$(DRIVER_DIR)/%.o: $(SRC_DRIVERS_DIR)/%.c
	@echo "Compiling $<..."
	$(SILENTMODE_FLAG) $(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INC) $< -o $@

# $^ dependency $@ target
$(OBJ_DIR)/$(NAME_STARTUP_FILE).o: Core/Startup/$(NAME_STARTUP_FILE).s
	@echo "Compiling $<..."
	$(SILENTMODE_FLAG) $(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $< -o $@

-include $(OBJ_CORE:.o=.d)
-include $(OBJ_DRIVERS:.o=.d)