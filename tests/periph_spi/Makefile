BOARD ?= samr21-xpro
include ../Makefile.tests_common

LOW_MEMORY_BOARDS := samd10-xmini

FEATURES_REQUIRED += periph_spi
FEATURES_OPTIONAL += periph_spi_on_qspi

ifeq (,$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
  FEATURES_OPTIONAL += periph_spi_reconfigure
endif

USEMODULE += xtimer
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += schedstatistics

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include
