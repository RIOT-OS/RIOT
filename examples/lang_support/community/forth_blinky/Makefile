# name of your application
APPLICATION = ficl_blinky

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# select ficl package
USEPKG += ficl

# Modules to include:
USEMODULE += ztimer_msec

# For now `examples/%/tests" still rely on the test applicaton being reset after
# a terminal is opened to synchronize.
TESTRUNNER_RESET_AFTER_TERM ?= 1

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

# generate .f.h header files of .f files
BLOBS += $(wildcard *.f)

include $(RIOTBASE)/Makefile.include
