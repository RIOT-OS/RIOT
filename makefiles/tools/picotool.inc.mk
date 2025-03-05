FLASHER ?= $(RIOTTOOLS)/picotool/picotool.sh

FLASHFILE ?= $(ELFFILE)

FFLAGS ?= $(FLASHFILE)

ifeq ($(RIOTTOOLS)/picotool/picotool.sh,$(FLASHER))
  FLASHDEPS += $(RIOTTOOLS)/picotool/picotool
endif
