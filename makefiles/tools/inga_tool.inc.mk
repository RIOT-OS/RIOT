export FLASHER = $(RIOTBASE)/dist/tools/inga_tool/inga_tool
# Using the FFLAGS here to append the port for the inga_tool and avrdude
export FFLAGS = -d $(PORT) -r && avrdude -c avr109 -p m1284p -P $(PORT) -b 230400 -u -U flash:w:$(HEXFILE)
FLASHDEPS += $(RIOTBASE)/dist/tools/inga_tool/inga_tool
