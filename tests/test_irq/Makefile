valgrind: CFLAGS += -g
valgrind: CFLAGS += -DHAVE_VALGRIND_VALGRIND_H

# name of your project
export PROJECT = test_irq
#
# for easy switching of boards
export BOARD ?= native

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE = $(CURDIR)/../..

ifeq (,$(findstring native,$(BOARD)))
	include $(RIOTBASE)/Makefile.unsupported
else

## Modules to include. 

USEMODULE += auto_init
USEMODULE += hwtimer
USEMODULE += uart0
USEMODULE += posix

include $(RIOTBASE)/Makefile.include
valgrind: all
endif
