BOARD ?= samr21-xpro
include ../Makefile.tests_common

FEATURES_REQUIRED += periph_spi
FEATURES_OPTIONAL += periph_spi_reconfigure

USEMODULE += xtimer
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += schedstatistics

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include
