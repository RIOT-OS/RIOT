APPLICATION = struct_tm_utility
include ../Makefile.tests_common

BOARD_INSUFFICIENT_RAM := stm32f0discovery

DISABLE_MODULE += auto_init

USEMODULE += shell
USEMODULE += uart0
USEMODULE += timex

# The MSP-430 toolchain lacks sscanf:
BOARD_BLACKLIST := chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

# Too little RAM
BOARD_BLACKLIST += redbee-econotag

include $(RIOTBASE)/Makefile.include
