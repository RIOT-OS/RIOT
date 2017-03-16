APPLICATION = pthread_rwlock
include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-mega2560 waspmote-pro arduino-uno arduino-duemilanove
# arduino mega2560 uno duemilanove: unknown type name: clockid_t

USEMODULE += pthread
USEMODULE += xtimer
USEMODULE += random

CFLAGS += -DNATIVE_AUTO_EXIT

BOARD_INSUFFICIENT_MEMORY += airfy-beacon chronos mbed_lpc1768 msb-430 msb-430h \
                             nrf51dongle nrf6310 nucleo32-f031 nucleo32-f042 nucleo32-l031 \
                             nucleo-f030 nucleo-f334 nucleo-l053 pca10000 pca10005 \
                             spark-core stm32f0discovery weio yunjia-nrf51822

include $(RIOTBASE)/Makefile.include
