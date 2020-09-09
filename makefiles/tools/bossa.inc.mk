BOSSA_VERSION ?= 1.9
FLASHFILE ?= $(BINFILE)
FLASHER ?= $(RIOTTOOLS)/bossa-$(BOSSA_VERSION)/bossac
FFLAGS_OPTS ?=

# Only use ROM_OFFSET with Bossa version 1.9
ifeq (1.9,$(BOSSA_VERSION))
  FFLAGS_OPTS += -o $(ROM_OFFSET)
endif

FFLAGS  ?= -p $(PROG_DEV) $(FFLAGS_OPTS) -e -i -w -v -b -R $(FLASHFILE)

# some arduino boards need to toggle the serial interface a little bit to get
# them ready for flashing...
ifneq (,$(BOSSA_ARDUINO_PREFLASH))
include $(RIOTMAKE)/tools/usb_board_reset.mk
endif

# if we go with the default (BOSSA shipped with RIOT), we download and build
# the tool if not already done
ifeq ($(RIOTTOOLS)/bossa-$(BOSSA_VERSION)/bossac,$(FLASHER))
  FLASHDEPS += $(FLASHER)
endif
