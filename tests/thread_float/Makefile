APPLICATION = thread_float
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := airfy-beacon arduino-uno arduino-duemilanove \
			     alliope-mini cc2650stk chronos maple-mini \
                             mbed_lpc1768 microbit msb-430 msb-430h nrf51dongle \
                             nrf6310 nucleo-f031k6 nucleo-f042k6 \
                             opencm9-04 pca10000 pca10005 spark-core \
                             stm32f0discovery weio yunjia-nrf51822

USEMODULE += printf_float
USEMODULE += xtimer

#DISABLE_MODULE += cortexm_fpu

include $(RIOTBASE)/Makefile.include
