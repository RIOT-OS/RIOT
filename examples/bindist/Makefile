# name of your application
APPLICATION = bindist

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# bindist specific stuff:
#
# build and use module "abc".
# use BINARY_DIRS instead of DIRS
BINARY_DIRS += abc
USEMODULE += abc

# list of files to include in binary distribution
# "bin/$(BOARD)/$(APPLICATION).elf" will automatically be added
DIST_FILES += Makefile
DIST_FILES += bin/$(BOARD)/abc.a

include $(RIOTBASE)/Makefile.include
