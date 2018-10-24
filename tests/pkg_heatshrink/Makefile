include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove \
                             arduino-uno \
                             nucleo-f031k6 \
                             #

TEST_ON_CI_WHITELIST += all

USEPKG += heatshrink
USEMODULE += embunit

include $(RIOTBASE)/Makefile.include
