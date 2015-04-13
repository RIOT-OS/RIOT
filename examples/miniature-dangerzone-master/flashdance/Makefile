####
#### Sample Makefile for building apps with the RIOT OS
####
#### The Sample Filesystem Layout is:
#### /this makefile
#### ../../RIOT 
#### 

# name of your project
export PROJECT = flashdance

# for easy switching of boards
ifeq ($(strip $(BOARD)),)
	export BOARD = msba2
endif

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE =$(CURDIR)/../../RIOT

# uncomment these lines if you want to use platform support from external
# repositories
#export RIOTCPU =$(CURDIR)/../../RIOT/thirdparty_cpu
#export RIOTBOARD =$(CURDIR)/../../RIOT/thirdparty_boards

## Modules to include. 

#USEMODULE += shell
#USEMODULE += uart0
#USEMODULE += posix
#USEMODULE += vtimer
#USEMODULE += sht11
#USEMODULE += ltc4150
#USEMODULE += cc110x
#USEMODULE += fat
USEMODULE += hwtimer

export INCLUDES = -I$(RIOTBOARD)/$(BOARD)/include -I$(RIOTBASE)/core/include -I$(RIOTCPU)/$(CPU)/include -I$(RIOTBASE)/sys/lib -I$(RIOTBASE)/sys/include/ -I$(RIOTBASE)/drivers/include/

include $(RIOTBASE)/Makefile.include
