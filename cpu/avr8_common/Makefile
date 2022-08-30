# define the module that is build (not strictly necessary)
MODULE = avr8_common

# add a list of subdirectories, that should also be build
DIRS = avr_libc_extra

include $(RIOTBASE)/Makefile.base

# Workaround bug in GCC 12 that assumes I/O addresses provided by the
# AVR libc as zero-sized.
CFLAGS += -Wno-array-bounds
