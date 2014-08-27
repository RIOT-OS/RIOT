APPLICATION = test_nrf_radio
include ../Makefile.tests_common

BOARD_BLACKLIST = mbed_lpc1768 arduino-due udoo qemu-i386 stm32f0discovery stm32f3discovery \
                  stm32f4discovery avsextrem chronos msb-430 msb430-common msb-430h msba2   \
                  msba2-common pttu qemu-i386 redbee-econotag telosb wsn430-common wsn430-v1_3b\
                  wsn430-v1_4 x86-multiboot-common z1 native

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += vtimer

include $(RIOTBASE)/Makefile.include
