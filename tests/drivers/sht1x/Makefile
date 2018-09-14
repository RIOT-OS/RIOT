include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno

DRIVER ?= sht11
BOARD ?= msba2

USEMODULE += $(DRIVER)
USEMODULE += shell
USEMODULE += saul_default
USEMODULE += shell_commands
USEMODULE += ps

include $(TESTBASE)/../Makefile.include
