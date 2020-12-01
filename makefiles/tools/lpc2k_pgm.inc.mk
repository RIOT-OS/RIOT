# Compile `lpc2k_pgm` when required
LPC2K_PGM = $(RIOTTOOLS)/lpc2k_pgm/bin/lpc2k_pgm
FLASHDEPS += $(LPC2K_PGM)

FLASHER ?= $(LPC2K_PGM)

FLASHFILE ?= $(HEXFILE)
FFLAGS ?= $(PROG_DEV) $(FLASHFILE)
