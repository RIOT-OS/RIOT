RIOT_EDBG = $(RIOTBASE)/dist/tools/edbg/edbg
EDBG ?= $(RIOT_EDBG)
FLASHER ?= $(EDBG)
OFLAGS ?= -O binary
HEXFILE = $(ELFFILE:.elf=.bin)
FFLAGS ?= $(EDBG_ARGS) -t $(EDBG_DEVICE_TYPE) -b -e -v -p -f $(HEXFILE)

ifeq ($(RIOT_EDBG),$(FLASHER))
  FLASHDEPS += $(RIOT_EDBG)
endif
