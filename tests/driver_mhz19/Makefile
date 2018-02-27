include ../Makefile.tests_common

USEMODULE += mhz19
USEMODULE += xtimer

# set default device parameters in case they are undefined
TEST_UART ?= 1

# export parameters
CFLAGS += -DTEST_UART=$(TEST_UART)

include $(RIOTBASE)/Makefile.include
