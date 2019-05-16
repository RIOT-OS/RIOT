include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-nano arduino-uno

FORCE_ASSERTS = 1
USEMODULE += event
USEMODULE += xtimer
USEMODULE += core_thread_flags

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include
