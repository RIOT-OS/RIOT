BOARD ?= b-l072z-lrwan1

include ../Makefile.tests_common

USEMODULE += od
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

DRIVER ?= sx1276

# use SX1276 by default
USEMODULE += $(DRIVER)

include $(RIOTBASE)/Makefile.include
