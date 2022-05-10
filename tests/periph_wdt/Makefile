BOARD ?= nucleo-l152re
include ../Makefile.tests_common

FEATURES_REQUIRED += periph_wdt

USEMODULE += xtimer
USEMODULE += shell

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
