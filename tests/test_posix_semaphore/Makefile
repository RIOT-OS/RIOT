export APPLICATION = test_posix_semaphore
include ../Makefile.tests_common

BOARD_BLACKLIST := mbed_lpc1768 redbee-econotag chronos
# mbed_lpc1768: not enough RAM
# redbee-econotag: not enough RAM
# chronos: not enough RAM

USEMODULE += posix

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include
