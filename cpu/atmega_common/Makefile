# define the module that is build (not strictly necessary)
MODULE = atmega_common

# add a list of subdirectories, that should also be build
DIRS = periph $(RIOTCPU)/avr8_common/

include $(RIOTBASE)/Makefile.base

# Workaround bug in GCC 12 that assumes I/O addresses provided by the
# AVR libc as zero-sized.
CFLAGS += -Wno-array-bounds
