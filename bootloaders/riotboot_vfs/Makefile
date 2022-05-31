# Default RIOT bootloader
APPLICATION = riotboot_vfs

USEMODULE += auto_init
USEMODULE += riotboot_vfs
USEMODULE += vfs_default

# In riotboot we do everything on the ISR stack
DISABLE_MODULE += core_thread
CFLAGS += -DISR_STACKSIZE=2048

include ../riotboot_common.mk
