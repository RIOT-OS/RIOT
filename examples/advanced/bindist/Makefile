# name of your application
APPLICATION = bindist

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# bindist specific stuff:
#
# build and use module "abc".
# use BIN_DIRS instead of DIRS, BIN_USEMODULE instead of USEMODULE,
# to flag that the directory should only be included when doing a normal build,
# and that the resulting .a should be saved when doing "make bindist"
BIN_DIRS += abc
BIN_USEMODULE += abc                    # include "abc" module
BIN_USEMODULE += $(APPLICATION_MODULE)  # include application module

# list of extra files to include in binary distribution
DIST_FILES += Makefile

include $(RIOTBASE)/Makefile.include
