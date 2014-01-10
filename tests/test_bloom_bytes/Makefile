# name of your project
export PROJECT = test_bloom

# for easy switching of boards
export BOARD ?= native

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE = $(CURDIR)/../..

ifneq (,$(findstring msb-430,$(BOARD)))
include $(RIOTBASE)/Makefile.unsupported
else

## Modules to include.
USEMODULE += hashes
USEMODULE += bloom
USEMODULE += random

include $(RIOTBASE)/Makefile.include
endif
