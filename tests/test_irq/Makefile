# name of your application
export PROJECT = test_irq
#
# for easy switching of boards
export BOARD ?= native

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE = $(CURDIR)/../..

ifeq (,$(filter native,$(BOARD)))
	include $(RIOTBASE)/Makefile.unsupported
else

## Modules to include.

USEMODULE += auto_init
USEMODULE += hwtimer
USEMODULE += posix

include $(RIOTBASE)/Makefile.include
endif
