APPLICATION = driver_io1_xplained
include ../Makefile.tests_common

USEMODULE += io1_xplained
USEMODULE += xtimer
USEMODULE += printf_float

# set default device parameters in case they are undefined
TEST_ADDR ?= 0x07

# export parameters
CFLAGS += -DTEST_ADDR=$(TEST_ADDR)

include $(RIOTBASE)/Makefile.include
