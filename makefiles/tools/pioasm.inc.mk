export PIOASM ?= $(RIOTTOOLS)/pioasm/pioasm

ifeq ($(RIOTTOOLS)/pioasm/pioasm,$(PIOASM))
  BUILDDEPS += $(RIOTTOOLS)/pioasm/pioasm
endif
