APPLICATION = bloom
include ../Makefile.tests_common

BOARD_INSUFFICIENT_RAM := chronos mbed_lpc1768 msb-430 msb-430h redbee-econotag \
                          telosb wsn430-v1_3b wsn430-v1_4 z1 stm32f0discovery \
                          stm32f3discovery pca10000 pca10005 yunjia-nrf51822

BOARD_BLACKLIST := arduino-mega2560
# arduino-mega2560: Errors in assembly, e.g:
#                   Error: value of 105617 too large for field of 2 bytes at 20018

USEMODULE += hashes
USEMODULE += bloom

APPDEPS = $(BINDIR)projdeps/sets.h

INCLUDES += -I$(BINDIR)projdeps

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include

$(BINDIR)projdeps/sets.h: generate_sets.py words.txt.gz
	mkdir -p ${@D}
	./generate_sets.py words.txt.gz $@
