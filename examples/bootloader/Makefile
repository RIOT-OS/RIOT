# name of your application
APPLICATION = bootloader

# If no BOARD is found in the environment, use this default:
BOARD ?= iotlab-m3

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#CFLAGS += -DDEVELHELP

# Mark this example as the bootloader
BOOTLOADER = 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Use fw_slots module to manage images on ROM
USEMODULE += fw_slots

include $(RIOTBASE)/Makefile.include
