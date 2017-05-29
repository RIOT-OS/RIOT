# name of your application
export PROJECT = test_easy_cpp_mix

# for easy switching of boards
export BOARD ?= native

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE = $(CURDIR)/../..

BOARD_CC=""
ifneq (,$(filter native, $(BOARD)))
    BOARD_CC = g++
else 
    ifneq (,$(filter msba2, $(BOARD)))
        BOARD_CC = arm-none-eabi-g++
    else
        include $(RIOTBASE)/Makefile.unsupported
    endif
endif

# switches for c++/c mixedmode
export CPPMIX = 1
export CPPMIXCC = ${BOARD_CC}
#export CPPMIXEXTRALIBS
export CPPMIXCFLAGS = -std=c++11

## Modules to include.
USEMODULE += posix

INCLUDES += -I$(CURDIR)/

include $(RIOTBASE)/Makefile.include
