####
#### Sample Makefile for building apps with the RIOT OS
####
#### The Sample Filesystem Layout is:
#### /this makefile
#### ../../RIOT 
#### 

# name of your project
export PROJECT = foobar

# for easy switching of boards
export BOARD ?= native

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE = $(CURDIR)/../../RIOT

# uncomment these lines if you want to use platform support from external
# repositories
#export RIOTCPU =$(CURDIR)/../../RIOT/thirdparty_cpu
#export RIOTBOARD =$(CURDIR)/../../RIOT/thirdparty_boards

# uncomment this to enable scheduler statistics for ps
#CFLAGS += -DSCHEDSTATISTICS

# If you want to use valgrind, you should recompile native with either
# HAVE_VALGRIND_H or HAVE_VALGRIND_VALGRIND_H depending on the location
# of the valgrind header (i.e. <valgrind.h> or <valgrind/valgrind.h>)
# For more information about the valgrind support of RIOT read:
# RIOT/cpu/native/README
#CFLAGS += -DHAVE_VALGRIND_VALGRIND_H
#CFLAGS += -DHAVE_VALGRIND_H

# Uncomment this to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#CFLAGS += -DDEVELHELP

## Modules to include. 

#USEMODULE += shell
#USEMODULE += uart0
#USEMODULE += posix
#USEMODULE += vtimer
#USEMODULE += sht11
#USEMODULE += ltc4150
#USEMODULE += cc110x
#USEMODULE += fat

export INCLUDES = -I$(RIOTBOARD)/$(BOARD)/include -I$(RIOTBASE)/core/include -I$(RIOTCPU)/$(CPU)/include -I$(RIOTBASE)/sys/lib -I$(RIOTBASE)/sys/include/ -I$(RIOTBASE)/drivers/include/

export QUIET := 1

include $(RIOTBASE)/Makefile.include
