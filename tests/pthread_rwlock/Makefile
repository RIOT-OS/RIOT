APPLICATION = pthread_rwlock
include ../Makefile.tests_common

USEMODULE += pthread
USEMODULE += vtimer
USEMODULE += random

DISABLE_MODULE += auto_init

CFLAGS += -DNATIVE_AUTO_EXIT

BOARD_INSUFFICIENT_RAM += chronos mbed_lpc1768 msb-430 msb-430h stm32f0discovery \
                          pca10000 pca10005

include $(RIOTBASE)/Makefile.include
