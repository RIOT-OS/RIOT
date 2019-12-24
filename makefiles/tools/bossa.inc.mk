FLASHFILE ?= $(BINFILE)
FLASHER ?= $(RIOTTOOLS)/bossa/bossac
FFLAGS  ?= -p $(PROG_DEV) -e -i -w -v -b -R $(FLASHFILE)

# some arduino boards need to toggle the serial interface a little bit to get
# them ready for flashing...
ifneq (,$(BOSSA_ARDUINO_PREFLASH))
  ifeq ($(OS),Linux)
    STTY_FLAG = -F
  else ifeq ($(OS),Darwin)
    STTY_FLAG = -f
  endif

  PREFLASHER ?= stty
  PREFFLAGS  ?= $(STTY_FLAG) $(PROG_DEV) raw ispeed 1200 ospeed 1200 cs8 -cstopb ignpar eol 255 eof 255
  FLASHDEPS += preflash

endif

# if we go with the default (BOSSA shipped with RIOT), we download and build
# the tool if not already done
ifeq ($(RIOTTOOLS)/bossa/bossac,$(FLASHER))
  FLASHDEPS += $(RIOTTOOLS)/bossa/bossac
endif
