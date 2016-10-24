# name of your application
APPLICATION = bootloader

# If no BOARD is found in the environment, use this default:
BOARD ?= iotlab-m3

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Mark this example as the bootloader
BOOTLOADER = 1

# Activate interactive bootloader, if no specified
INTERACTIVE_BOOTLOADER ?= 1
CFLAGS += -DINTERACTIVE_BOOTLOADER=$(INTERACTIVE_BOOTLOADER)

# Import metadata space size, otherwise it will be defaulted to 0x100 by 
# the code
CFLAGS += -DFW_METADATA_SPACE=$(FW_METADATA_SPACE)

# Increase stack memory for flashpage requirements
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(3*THREAD_STACKSIZE_DEFAULT\)

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Use fw_slots module to manage images on ROM
USEMODULE += fw_slots

# Add also the shell, some shell commands
ifeq ($(INTERACTIVE_BOOTLOADER),1)
USEMODULE += shell
USEMODULE += shell_commands
endif

include $(RIOTBASE)/Makefile.include
