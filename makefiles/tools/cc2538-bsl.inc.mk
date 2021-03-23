CC2538_BSL_FLASHFILE ?= $(BINFILE)
FLASHFILE ?= $(CC2538_BSL_FLASHFILE)
CC2538_BSL ?= $(RIOTTOOLS)/cc2538-bsl/cc2538-bsl.py
PROG_BAUD ?= 500000	# default value in cc2538-bsl

# Warn about deprecated variables
ifneq (,$(FFLAGS_OPTS))
  $(warning Warning! FFLAGS_OPTS is deprecated use CC2538_BSL_FLAGS_OPTS)
  CC2538_BSL_FLAGS_OPTS ?= $(FFLAGS_OPTS)
endif

CC2538_BSL_FLAGS_OPTS ?=
CC2538_BSL_FLAGS  = $(if $(IMAGE_OFFSET), -a $(shell printf "0x%08x" $$(($(IMAGE_OFFSET) + $(ROM_START_ADDR)))))
CC2538_BSL_FLAGS += -p "$(PROG_DEV)" $(CC2538_BSL_FLAGS_OPTS) --write-erase -v -b $(PROG_BAUD) $(FLASHFILE)

FLASHER ?= $(CC2538_BSL)
FFLAGS ?= $(CC2538_BSL_FLAGS)

RESET ?= $(FLASHER) -p "$(PROG_DEV)" $(CC2538_BSL_FLAGS_OPTS) -b $(PROG_BAUD)

ifeq ($(RIOTTOOLS)/cc2538-bsl/cc2538-bsl.py,$(FLASHER))
  FLASHDEPS += $(FLASHER)
endif
