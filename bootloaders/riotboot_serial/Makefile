# Default RIOT bootloader
APPLICATION = riotboot_serial

# Include serial bootloader functionality
USEMODULE += riotboot_serial

# RIOT codebase
RIOTBASE ?= $(CURDIR)/../../

include $(RIOTBASE)/Makefile.include

# limit riotboot bootloader size
RIOTBOOT_SERIAL_LEN ?= $(RIOTBOOT_LEN)
ROM_LEN := $(RIOTBOOT_SERIAL_LEN)
