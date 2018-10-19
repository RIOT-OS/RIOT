include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove \
                             arduino-leonardo \
                             arduino-nano \
                             arduino-uno \
                             i-nucleo-lrwan1 \
                             nucleo-f031k6 \
                             nucleo-f042k6 \
                             nucleo-l031k6 \
                             nucleo-l053r8 \
                             nucleo-f030r8 \
                             stm32f0discovery \

BOARD_BLACKLIST := chronos \
                   msb-430 \
                   msb-430h \
                   telosb \
                   wsn430-v1_3b \
                   wsn430-v1_4 \
                   z1 \

TEST_ON_CI_WHITELIST += all

USEMODULE += spiffs
USEMODULE += embunit

include $(RIOTBASE)/Makefile.include
