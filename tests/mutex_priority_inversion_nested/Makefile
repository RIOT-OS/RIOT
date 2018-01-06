APPLICATION = mutex_priority_inversion_nested
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031 nucleo32-f042 nucleo32-l031 nucleo-f030 \
                             nucleo-l053 stm32f0discovery weio

USEMODULE += core_priority_inheritance
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
