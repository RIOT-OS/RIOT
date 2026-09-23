MGBA ?= mgba
FLASHFILE ?= $(BINFILE)

ifeq (,$(EMULATOR_TMP_DIR))
  EMULATOR_TMP_DIR := $(shell mktemp -td riot_$(APPLICATION)_$(BOARD).XXXXX)
endif

# mGBA prints the messages written to its debug registers (see the stdio_mgba
# module) to stdout, provided that logging to stdout is enabled and the log
# level of the 'gba.debug' category includes the debug level (16).
MGBA_LOG_LEVEL ?= 16
MGBA_FLAGS ?= -C logToStdout=1 -C logLevel.gba.debug=$(MGBA_LOG_LEVEL)

# mGBA GUI (there is no headless flag).
MGBA_SHOW_GUI ?= 1
ifneq (1,$(MGBA_SHOW_GUI))
  export QT_QPA_PLATFORM = offscreen
  export QT_LOGGING_RULES = default.warning=false
  export SDL_VIDEODRIVER = dummy
  export SDL_VIDEO_DRIVER = dummy
  MGBA_FLAGS += -C logLevel.platform.qt=1
endif

# Configure emulator variables.
EMULATOR ?= $(MGBA)
EMULATOR_FLAGS ?= $(MGBA_FLAGS) $(FLASHFILE)

# Configure the terminal access.
TERMPROG = $(EMULATOR)
TERMFLAGS = $(EMULATOR_FLAGS)

# Configure the debugger.
GDB_REMOTE ?= :2345

DEBUGSERVER ?= $(EMULATOR)
DEBUGSERVER_FLAGS ?= $(MGBA_FLAGS) -g $(FLASHFILE)

DEBUGGER_FLAGS ?= $(BOARD) $(APPDIR) $(DEBUG_ELFFILE) $(GDB_REMOTE) $(EMULATOR_TMP_DIR)
DEBUGGER ?= $(RIOTTOOLS)/emulator/debug.sh

# No flasher available with mGBA emulator.
FLASHER ?=
FFLAGS ?=
