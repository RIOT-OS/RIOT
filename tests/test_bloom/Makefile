export PROJECT = test_bloom
include ../Makefile.tests_common

BOARD_BLACKLIST := chronos mbed_lpc1768 msb-430 msb-430h redbee-econotag \
                   telosb wsn430-v1_3b wsn430-v1_4 z1
# chronos: not enough RAM
# mbed_lpc1768: not enough RAM
# msb-430: not enough RAM
# msb-430h: not enough RAM
# redbee-econotag: not enough RAM
# telosb: not enough RAM
# wsn430-v1_3b: not enough RAM
# wsn430-v1_4: not enough RAM
# z1: not enough RAM

USEMODULE += hashes
USEMODULE += bloom

export PROJDEPS = sets.h

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include

sets.h: generate_sets.py words.txt.gz
	./generate_sets.py 10000 20
