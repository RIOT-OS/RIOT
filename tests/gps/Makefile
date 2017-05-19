# name of your application
APPLICATION = gps

include ../Makefile.tests_common

USEMODULE += gps_serial

# The MSP-430 toolchain lacks mktime and NAN
BOARD_BLACKLIST += chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

# mips-malta is missing uart_init()
BOARD_BLACKLIST += mips-malta

include $(RIOTBASE)/Makefile.include
