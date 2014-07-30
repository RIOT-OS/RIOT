APPLICATION = test_condition_variable
include ../Makefile.tests_common

BOARD_INSUFFICIENT_RAM := stm32f0discovery

USEMODULE += posix
USEMODULE += pthread
USEMODULE += vtimer

CFLAGS += -DNATIVE_AUTO_EXIT

include $(RIOTBASE)/Makefile.include
