include ../Makefile.tests_common

TEST_ON_CI_WHITELIST += all

# No 8 bit and 16 bit support
BOARD_BLACKLIST := arduino-duemilanove arduino-leonardo \
                   arduino-mega2560 arduino-nano \
                   arduino-uno chronos jiminy-mega256rfr2 mega-xplained \
                   msb-430 msb-430h telosb waspmote-pro wsn430-v1_3b \
                   wsn430-v1_4 z1

USEPKG += nanocbor
# Used for verification
USEMODULE += embunit

include $(RIOTBASE)/Makefile.include
