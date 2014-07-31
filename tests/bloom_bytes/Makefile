APPLICATION = bloom_bytes
include ../Makefile.tests_common

BOARD_INSUFFICIENT_RAM := stm32f0discovery

USEMODULE += hashes
USEMODULE += bloom
USEMODULE += random

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include
