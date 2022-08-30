# define the module that is build
MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS = periph $(RIOTCPU)/avr8_common/

ifneq (,$(findstring a1,$(shell echo $(CPU_MODEL) | cut -c8-)))
    DIRS += ebi
endif

include $(RIOTBASE)/Makefile.base

# Workaround bug in GCC 12 that assumes I/O addresses provided by the
# AVR libc as zero-sized.
CFLAGS += -Wno-array-bounds
