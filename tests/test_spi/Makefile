APPLICATION = test_spi
include ../Makefile.tests_common

# If no BOARD is found in the environment, use this default:
BOARD ?= arduino-due

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Use of software timer
export USEMODULE += vtimer


DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include
