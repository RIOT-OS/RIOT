# name of your project
export PROJECT = ipc_pingpong

# for easy switching of boards
ifeq ($(strip $(BOARD)),)
	export BOARD = native
endif

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE = $(CURDIR)/../..

include $(RIOTBASE)/Makefile.include
