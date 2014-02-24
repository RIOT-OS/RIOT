# name of your project
export PROJECT = test_condition_variable

# for easy switching of boards
export BOARD ?= native

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE = $(CURDIR)/../RIOT

## Modules to include.
USEMODULE += vtimer
USEMODULE += condition_variable

export INCLUDES = -I$(RIOTBASE)/sys/condition_variable/include

include $(RIOTBASE)/Makefile.include
