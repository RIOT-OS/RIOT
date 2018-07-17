include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-mega2560 chronos msb-430 msb-430h telosb \
                   wsn430-v1_3b wsn430-v1_4 z1 waspmote-pro arduino-uno \
                   arduino-duemilanove jiminy-mega256rfr2 mega-xplained
BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6 nucleo-f042k6 nucleo-l031k6 nucleo-f030r8 \
                             nucleo-f303k8 nucleo-f334r8 nucleo-l053r8 \
                             stm32f0discovery

USEMODULE += gnrc_ipv6
USEMODULE += gnrc_sock_udp
USEMODULE += nhdp

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
