APPLICATION = posix_semaphore
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := msb-430 msb-430h mbed_lpc1768 chronos stm32f0discovery \
                          pca10000 pca10005 weio yunjia-nrf51822 nrf6310 spark-core \
                          nucleo-f334 nucleo-f030 nucleo32-f042 nucleo32-f031 \
                          nucleo-l053

USEMODULE += fmt
USEMODULE += posix_semaphore

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
