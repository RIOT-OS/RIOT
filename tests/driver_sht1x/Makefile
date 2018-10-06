BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno

DRIVER ?= sht11
BOARD ?= msba2

USEMODULE += $(DRIVER)
USEMODULE += shell
USEMODULE += saul_default
USEMODULE += shell_commands
USEMODULE += ps

include ../Makefile.tests_common
include $(RIOTBASE)/Makefile.include
