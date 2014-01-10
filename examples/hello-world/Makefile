# name of your project
export PROJECT = hello-world

# for easy switching of boards
ifeq ($(strip $(BOARD)),)
	export BOARD = native
endif

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE = $(CURDIR)/../..

export QUIET := 1

include $(RIOTBASE)/Makefile.include
