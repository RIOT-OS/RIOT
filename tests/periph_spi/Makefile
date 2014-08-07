export APPLICATION = periph_spi
include ../Makefile.tests_common

BOARD_BLACKLIST := chronos mbed_lpc1768 msb-430 msb-430h native qemu-i386 redbee-econotag telosb \
                   wsn430-v1_3b wsn430-v1_4 z1
# all listed boards: no periph_conf.h defined,

USEMODULE += shell
USEMODULE += shell_commands

include $(RIOTBASE)/Makefile.include
