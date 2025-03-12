
FLASHER ?= $(RIOTTOOLS)/elf2uf2/elf2uf2.sh

FLASHFILE ?= $(EXECUTABLE)

FFLAGS ?= $(FLASHFILE)

#yes elf2uf2 not elf2uf2.sh
#the first should be downloaded from git and build
#the secend is provided by RIOT code
#FLASHDEPS += $(RIOTTOOLS)/elf2uf2/elf2uf2

ifeq ($(RIOTTOOLS)/elf2uf2/elf2uf2.sh,$(FLASHER))
  FLASHDEPS += $(RIOTTOOLS)/elf2uf2/elf2uf2
endif
