APPLICATION = posix_semaphore
include ../Makefile.tests_common

BOARD_INSUFFICIENT_RAM := msb-430 msb-430h mbed_lpc1768 redbee-econotag chronos stm32f0discovery \
                          pca10000 pca10005 yunjia-nrf51822 spark-core

USEMODULE += posix

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include
