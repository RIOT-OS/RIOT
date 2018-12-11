include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno

FORCE_ASSERTS = 1
USEMODULE += event_callback
USEMODULE += event_timeout

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include
