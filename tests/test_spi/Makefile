APPLICATION = test_spi
include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Use of software timer
USEMODULE += vtimer

DISABLE_MODULE += auto_init

# Blacklist boards
BOARD_BLACKLIST := native arduino-due avsextrem chronos mbed_lpc1768 msb-430h msba2 redbee-econotag \
                   telosb wsn430-v1_3b wsn430-v1_4 msb-430 pttu udoo qemu-i386 z1 stm32f0discovery
                   
include $(RIOTBASE)/Makefile.include
