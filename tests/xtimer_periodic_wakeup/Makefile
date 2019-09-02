include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-leonardo arduino-nano \
                             arduino-uno chronos

USEMODULE += xtimer

# This test randomly fails on `native` so disable it from CI
TEST_ON_CI_BLACKLIST += native

include $(RIOTBASE)/Makefile.include
