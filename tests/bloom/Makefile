APPLICATION = bloom
include ../Makefile.tests_common

BOARD_INSUFFICIENT_RAM := chronos mbed_lpc1768 msb-430 msb-430h redbee-econotag \
                          telosb wsn430-v1_3b wsn430-v1_4 z1 stm32f0discovery \
                          stm32f3discovery

USEMODULE += hashes
USEMODULE += bloom

APPDEPS = $(BINDIR)projdeps/sets.h

INCLUDES += -I$(BINDIR)projdeps

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include

$(BINDIR)projdeps/sets.h: generate_sets.py words.txt.gz
	mkdir -p ${@D}
	./generate_sets.py words.txt.gz $@
