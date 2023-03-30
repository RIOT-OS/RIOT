BOARD ?= b-l072z-lrwan1

include ../Makefile.drivers_common

USEMODULE += od
USEMODULE += shell_cmds_default
USEMODULE += ps

DRIVER ?= sx1276

# use SX1276 by default
USEMODULE += $(DRIVER)

include $(RIOTBASE)/Makefile.include
