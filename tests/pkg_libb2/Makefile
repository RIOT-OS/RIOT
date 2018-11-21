include ../Makefile.tests_common

# BLAKE2s + BLAKE2 is too big for these boards
BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno chronos \
                             mega-xplained msb-430 msb-430h nucleo-f031k6 \
                             nucleo-f042k6 nucleo-l031k6 telosb waspmote-pro \
                             wsn430-v1_3b wsn430-v1_4 z1

TEST_ON_CI_WHITELIST += all

USEPKG += libb2
USEMODULE += embunit

include $(RIOTBASE)/Makefile.include
