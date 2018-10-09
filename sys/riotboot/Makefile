SUBMODULES := 1

ifneq (,$(filter riotboot_slot,$(USEMODULE)))
    CFLAGS += -DSLOT0_OFFSET=$(SLOT0_OFFSET)
endif

include $(RIOTBASE)/Makefile.base
