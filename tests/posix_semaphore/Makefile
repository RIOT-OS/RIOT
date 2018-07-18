include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := chronos mbed_lpc1768 msb-430 msb-430h nrf6310 \
                             nucleo-f031k6 nucleo-f042k6 nucleo-l031k6 nucleo-f030r8 \
                             nucleo-f303k8 nucleo-f334r8 nucleo-l053r8 \
                             spark-core stm32f0discovery yunjia-nrf51822

USEMODULE += fmt
USEMODULE += posix_semaphore

DISABLE_MODULE += auto_init

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
