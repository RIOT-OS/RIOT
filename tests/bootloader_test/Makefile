# name of your application
APPLICATION = bootloader_test

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

include ../Makefile.tests_common

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEMODULE += firmware

APP_ID = 0xBB8
APP_VERSION = 0x1

# this test is supposed to always build an slotted image
all: combined

include $(RIOTBASE)/Makefile.include
