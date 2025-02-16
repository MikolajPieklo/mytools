
DEBUGINFO :=
OPTIMIZATION := 1
ifneq ($(MAKECMDGOALS), release)
$(info Added debug symbols)
DEBUGINFO += -DDEBUG -g3
OPTIMIZATION = 0
endif

SILENTMODE_FLAG :=
ifneq ($(SILENTMODE), yes)
SILENTMODE_FLAG := @
endif
