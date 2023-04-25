BOARD ?= samr21-xpro
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2s

USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ztimer

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include
