BOARD ?= iotlab-m3
include ../Makefile.tests_common

FEATURES_REQUIRED += periph_flashpage
FEATURES_OPTIONAL += periph_flashpage_raw
FEATURES_OPTIONAL += periph_flashpage_rwee

USEMODULE += od
USEMODULE += shell

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include
