# Default RIOT bootloader
APPLICATION = riotboot

# Include riotboot flash partition functionality
USEMODULE += riotboot_slot

# We don't want to re-configure any hardware
CFLAGS += -DDISABLE_BOARD_INIT
CFLAGS += -DDISABLE_CPU_INIT

include ../riotboot_common.mk
