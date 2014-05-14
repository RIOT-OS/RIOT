export APPLICATION = test_thread_cooperation
include ../Makefile.tests_common

BOARD_BLACKLIST := chronos msb-430 msb-430h mbed_lpc1768 redbee-econotag
# chronos: not enough RAM
# mbed_lpc1768: not enough RAM
# msb-430: not enough RAM
# msb-430h: not enough RAM
# redbee-econotag: not enough RAM

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include

