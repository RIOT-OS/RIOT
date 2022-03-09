# define the module that is build
MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS = periph $(RIOTCPU)/avr8_common/

ifneq (,$(findstring a1,$(shell echo $(CPU_MODEL) | cut -c8-)))
    DIRS += ebi
endif

include $(RIOTBASE)/Makefile.base
