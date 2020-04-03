FLASHFILE ?= $(BINFILE)
FLASHER = $(RIOTTOOLS)/cc2538-bsl/cc2538-bsl.py
FFLAGS_OPTS ?=
PROG_BAUD ?= 500000	# default value in cc2538-bsl
FFLAGS  = -p "$(PROG_DEV)" $(FFLAGS_OPTS) -e -w -v -b $(PROG_BAUD) $(FLASHFILE)

RESET ?= $(FLASHER) -p "$(PROG_DEV)" $(FFLAGS_OPTS)

ifeq ($(RIOTTOOLS)/cc2538-bsl/cc2538-bsl.py,$(FLASHER))
  FLASHDEPS += $(FLASHER)
endif
