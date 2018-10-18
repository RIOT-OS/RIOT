include ../Makefile.tests_common

# HACL* only compiles on 32bit platforms
BOARD_BLACKLIST := arduino-duemilanove arduino-mega2560 arduino-uno \
                   jiminy-mega256rfr2 mega-xplained waspmote-pro \
                   chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

BOARD_INSUFFICIENT_MEMORY := nucleo-f030r8 nucleo-f031k6 nucleo-f042k6 \
                             nucleo-l031k6 nucleo-l053r8 stm32f0discovery

TEST_ON_CI_WHITELIST += native

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(4*THREAD_STACKSIZE_DEFAULT\)

USEPKG += libcose
USEMODULE += libcose_crypt_hacl
USEMODULE += memarray
USEMODULE += embunit

include $(RIOTBASE)/Makefile.include
