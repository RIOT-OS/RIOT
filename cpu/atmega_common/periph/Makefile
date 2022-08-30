MODULE = atmega_common_periph
include $(RIOTMAKE)/periph.mk

# Workaround bug in GCC 12 that assumes I/O addresses provided by the
# AVR libc as zero-sized.
CFLAGS += -Wno-array-bounds
