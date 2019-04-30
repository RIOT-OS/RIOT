# Add a list of subdirectories, that should also be built:
DIRS += esp

ifeq ($(ENABLE_GDBSTUB), 1)
    DIRS += esp-gdbstub
endif

include $(RIOTBASE)/Makefile.base
