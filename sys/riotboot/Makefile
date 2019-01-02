SUBMODULES := 1

ifneq (,$(filter riotboot_slot,$(USEMODULE)))
    CFLAGS += -DSLOT0_OFFSET=$(SLOT0_OFFSET)
    CFLAGS += -DSLOT1_OFFSET=$(SLOT1_OFFSET)
    CFLAGS += -DNUM_SLOTS=$(NUM_SLOTS)
endif

include $(RIOTBASE)/Makefile.base
