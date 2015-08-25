APPLICATION = pthread_rwlock
include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-mega2560
# arduino-mega2560: unknown type name: clockid_t

USEMODULE += pthread
USEMODULE += vtimer
USEMODULE += random

DISABLE_MODULE += auto_init

CFLAGS += -DNATIVE_AUTO_EXIT

BOARD_INSUFFICIENT_RAM += chronos mbed_lpc1768 msb-430 msb-430h stm32f0discovery \
                          pca10000 pca10005 yunjia-nrf51822 spark-core nucleo-f334 \
                          airfy-beacon nrf51dongle nrf6310

include $(RIOTBASE)/Makefile.include
