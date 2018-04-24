CFLAGS += -DFIRMWARE_NUM_SLOTS=3
CFLAGS += -DSLOT0_SIZE=$(RIOTBOOT_SLOT0_SIZE)
CFLAGS += -DSLOT_SIZE=$(RIOTBOOT_FW_SLOT_SIZE)

SRC = firmware.c firmware_flashwrite.c

ifneq (,$(filter firmware_simple,$(USEMODULE)))
  SRC += firmware_simple.c
endif

include $(RIOTBASE)/Makefile.base
