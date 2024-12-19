# Default RIOT bootloader
APPLICATION = riotboot

# Include riotboot flash partition functionality
USEMODULE += riotboot_slot

# We don't want to re-configure any hardware
CFLAGS += -DDISABLE_BOARD_INIT=1
CFLAGS += -DDISABLE_CPU_INIT=1

include ../riotboot_common.mk
