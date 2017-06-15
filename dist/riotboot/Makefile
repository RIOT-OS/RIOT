APPLICATION = riotboot

BOARD ?= samr21-xpro

USEMODULE += firmware

CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE
DISABLE_MODULE += auto_init

RIOTBASE ?= $(CURDIR)/../..

include $(RIOTBASE)/Makefile.include

# limit bootloader size
# HACK: FIXME it does not work if there is an existing board bootlader
ROM_LEN := $(RIOTBOOT_SLOT0_SIZE)
