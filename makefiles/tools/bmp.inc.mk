FLASHER ?= $(RIOTTOOLS)/bmp/bmp.py
DEBUGGER ?= $(RIOTTOOLS)/bmp/bmp.py
RESET ?= $(RIOTTOOLS)/bmp/bmp.py

FLASHFILE ?= $(EXECUTABLE)

FFLAGS ?= $(BMP_OPTIONS) flash $(FLASHFILE)
DEBUGGER_FLAGS ?= $(BMP_OPTIONS) debug $(DEBUG_EXECUTABLE)
RESET_FLAGS ?= $(BMP_OPTIONS) reset
