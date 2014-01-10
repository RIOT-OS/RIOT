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

export PROJDEPS = sets.h

include $(RIOTBASE)/Makefile.include
endif

sets.h: generate_sets.py words.txt.gz
	./generate_sets.py 10000 20
