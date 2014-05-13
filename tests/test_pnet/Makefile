export PROJECT = test_pnet
include ../Makefile.tests_common

BOARD_BLACKLIST := chronos mbed_lpc1768 msb-430 msb-430h redbee-econotag \
                   telosb wsn430-v1_3b wsn430-v1_4 arduino-due
# chronos:          not enough RAM
# mbed_lpc1768:     see https://github.com/RIOT-OS/RIOT/issues/675
# msb-430:          see https://github.com/RIOT-OS/RIOT/issues/658
# msb-430h:         not enough RAM
# redbee-econotag:  not enough RAM
# telosb:           not enough RAM
# wsn430-v1_3b:     not enough RAM
# wsn430-v1_4:      not enough RAM

USEMODULE += posix
USEMODULE += pnet
USEMODULE += vtimer
USEMODULE += defaulttransceiver

include $(RIOTBASE)/Makefile.include
