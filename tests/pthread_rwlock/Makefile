APPLICATION = pthread_rwlock
include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-mega2560 waspmote-pro arduino-uno arduino-duemilanove
# arduino mega2560 uno duemilanove: unknown type name: clockid_t

USEMODULE += pthread
USEMODULE += xtimer
USEMODULE += random

CFLAGS += -DNATIVE_AUTO_EXIT

BOARD_INSUFFICIENT_MEMORY += chronos mbed_lpc1768 msb-430 msb-430h stm32f0discovery \
                          pca10000 pca10005 yunjia-nrf51822 spark-core nucleo-f334 \
                          airfy-beacon nrf51dongle nrf6310 weio nucleo-f030 \
                          nucleo32-f042 nucleo32-f031 nucleo-l053

include $(RIOTBASE)/Makefile.include
