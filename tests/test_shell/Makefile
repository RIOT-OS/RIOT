# name of your project
export PROJECT = test_shell

# for easy switching of boards
export BOARD ?= native

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE = $(CURDIR)/../..

## Modules to include.

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += posix

include $(RIOTBASE)/Makefile.include
