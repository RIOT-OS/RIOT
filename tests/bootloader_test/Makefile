# name of your application
APPLICATION = bootloader_test

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEMODULE += firmware

FW_APPID = 0xBB8
FW_VERSION = 0x1

BOARD_WHITELIST := samr21-xpro iotlab-m3

# this test is supposed to always build an slotted image
all: combined

include $(RIOTBASE)/Makefile.include
