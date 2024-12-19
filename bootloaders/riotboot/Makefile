# Default RIOT bootloader
APPLICATION = riotboot

# Include riotboot flash partition functionality
USEMODULE += riotboot_slot

# We don't want to re-configure any hardware
CFLAGS += -DRIOTBOOT_MINIMAL=1

include ../riotboot_common.mk
