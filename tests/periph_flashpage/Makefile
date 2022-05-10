BOARD ?= iotlab-m3
include ../Makefile.tests_common

FEATURES_REQUIRED += periph_flashpage
FEATURES_OPTIONAL += periph_flashpage_in_address_space
FEATURES_OPTIONAL += periph_flashpage_pagewise
FEATURES_OPTIONAL += periph_flashpage_rwee

USEMODULE += od
USEMODULE += shell

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
