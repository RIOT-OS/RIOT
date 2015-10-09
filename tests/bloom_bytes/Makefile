APPLICATION = bloom_bytes
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

USEMODULE += hashes
USEMODULE += bloom
USEMODULE += random
USEMODULE += xtimer

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include
