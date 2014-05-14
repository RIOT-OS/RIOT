APPLICATION = test_pthread_rwlock
include ../Makefile.tests_common

USEMODULE += pthread
USEMODULE += vtimer
USEMODULE += random

DISABLE_MODULE += auto_init

CFLAGS += -DNATIVE_AUTO_EXIT

# these boards provide too little RAM for the example to run
BOARD_BLACKLIST += chronos
BOARD_BLACKLIST += mbed_lpc1768
BOARD_BLACKLIST += msb-430
BOARD_BLACKLIST += msb-430h

include $(RIOTBASE)/Makefile.include
