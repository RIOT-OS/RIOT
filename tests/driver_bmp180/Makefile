include ../Makefile.tests_common

USEMODULE += bmp180
USEMODULE += ztimer
USEMODULE += ztimer_msec

# set default altitude
TEST_ALTITUDE ?= 158 # altitude in Polytechnique School campus

# export altitude parameter
CFLAGS += -DTEST_ALTITUDE=$(TEST_ALTITUDE)

include $(RIOTBASE)/Makefile.include
