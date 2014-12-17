export APPLICATION = periph_uart_blocking
include ../Makefile.tests_common

BOARD_BLACKLIST := chronos mbed_lpc1768 msb-430 msb-430h native qemu-i386 redbee-econotag telosb \
                   wsn430-v1_3b wsn430-v1_4 z1
# all listed boards: no periph_conf.h defined,

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include
