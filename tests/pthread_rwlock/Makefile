include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-duemilanove \
                   arduino-mega2560 \
                   arduino-uno \
                   jiminy-mega256rfr2 \
                   mega-xplained \
                   waspmote-pro
# arduino mega2560 uno duemilanove : unknown type name: clockid_t
# jiminy-mega256rfr2: unknown type name: clockid_t
# mega-xplained: unknown type name: clockid_t

USEMODULE += pthread
USEMODULE += xtimer
USEMODULE += random

BOARD_INSUFFICIENT_MEMORY += chronos msb-430 msb-430h nucleo-f031k6 \
                             nucleo-f042k6 nucleo-l031k6 nucleo-f030r8 \
                             nucleo-f303k8 nucleo-f334r8 nucleo-l053r8 \
                             stm32f0discovery

include $(RIOTBASE)/Makefile.include
