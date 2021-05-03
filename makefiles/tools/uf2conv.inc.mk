FLASHFILE ?= $(HEXFILE)

# Warn about deprecated variables
ifneq (,$(FFLAGS_OPTS))
  $(warning Warning! FFLAGS_OPTS is deprecated use UF2CONV_FLAGS)
  UF2CONV_FLAGS ?= $(FFLAGS_OPTS)
endif

FLASHER ?= $(RIOTTOOLS)/uf2/uf2conv.py
FFLAGS  ?= $(UF2CONV_FLAGS) $(FLASHFILE)

PREFLASH_DELAY ?= 2

ifeq ($(RIOTTOOLS)/uf2/uf2conv.py,$(FLASHER))
  FLASHDEPS += $(FLASHER)
endif
