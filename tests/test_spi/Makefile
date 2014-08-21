# name of your application
APPLICATION = test_spi

# If no BOARD is found in the environment, use this default:
BOARD ?= arduino-due

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Use of software timer
export USEMODULE += vtimer

DISABLE_MODULE += auto_init

# Blacklist boards
BOARD_BLACKLIST := native avsextrem chronos mbed_lpc1768 msb-430h msba2 redbee-econotag \
                   telosb wsn430-v1_3b wsn430-v1_4 msb-430 pttu udoo qemu-i386 z1 stm32f0discovery \
                   stm32f3discovery stm32f4discovery pca10000 pca10005

include ../Makefile.tests_common
include $(RIOTBASE)/Makefile.include
