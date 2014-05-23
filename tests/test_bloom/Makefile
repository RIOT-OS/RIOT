export APPLICATION = test_bloom
include ../Makefile.tests_common

BOARD_INSUFFICIENT_RAM := chronos mbed_lpc1768 msb-430 msb-430h redbee-econotag \
                          telosb wsn430-v1_3b wsn430-v1_4 z1

USEMODULE += hashes
USEMODULE += bloom

export PROJDEPS = sets.h

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include

sets.h: generate_sets.py words.txt.gz
	./generate_sets.py 10000 20
