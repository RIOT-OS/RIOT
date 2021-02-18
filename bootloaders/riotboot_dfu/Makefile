# Default RIOT bootloader
APPLICATION = riotboot_dfu

# Default testing board
BOARD ?= samr21-xpro

# Select the boards with riotboot feature
FEATURES_REQUIRED += riotboot

# Set RIOTBOOT_BUILD to indicate a riotboot application build
RIOTBOOT_BUILD = 1
# Provide a define to detect if building the bootloader
CFLAGS += -DRIOTBOOT

# Disable unused modules
CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE
DISABLE_MODULE += core_init core_msg core_panic
DISABLE_MODULE += auto_init auto_init_%

# avoid using stdio
USEMODULE += stdio_null
# Add RIOTBOOT USB DFU integration
USEMODULE += riotboot_usb_dfu

# Use xtimer for scheduled reboot
USEMODULE += xtimer

# RIOT codebase
RIOTBASE ?= $(CURDIR)/../../

# USB device vendor and product ID
# pid.codes test VID/PID, not globally unique

# The VID/PID pair allocated for the RIOT bootloader
# as allocated on https://pid.codes/1209/7D02/
USB_VID ?= 1209
USB_PID ?= 7D02

include $(RIOTBASE)/Makefile.include

# limit riotboot bootloader size
# TODO: Manage to set this variable for boards which already embed a
# bootloader, currently it will be overwritten
ROM_LEN := $(RIOTBOOT_LEN)
