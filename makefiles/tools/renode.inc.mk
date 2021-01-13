RENODE ?= renode
RENODE_IMAGE_FILE ?= $(ELFFILE)
RENODE_BOARD_CONFIG ?= $(BOARDDIR)/dist/board.resc

# Global build system configurations
FLASHFILE ?= $(ELFFILE)
EMULATORDEPS += $(RENODE_BOARD_CONFIG)

# Use renode interactive commands to specify the image file and board config
RENODE_CONFIG_FLAGS += -e "set image_file '$(RENODE_IMAGE_FILE)'"
RENODE_CONFIG_FLAGS += -e "include @$(RENODE_BOARD_CONFIG)"
EMULATOR_PIDFILE ?=
ifneq (,$(EMULATOR_PIDFILE))
  $(info Using Renode pid file $(EMULATOR_PIDFILE))
  RENODE_CONFIG_FLAGS += --pid-file $(EMULATOR_PIDFILE)
endif

# Renode logging configuration
RENODE_SHOW_LOG ?= 0
ifneq (1,$(RENODE_SHOW_LOG))
  RENODE_CONFIG_FLAGS += --hide-log
endif
RENODE_LOG_LEVEL ?= 2  # Warning level
RENODE_CONFIG_FLAGS += -e "logLevel $(RENODE_LOG_LEVEL)"

# Renode GUI
RENODE_SHOW_GUI ?= 0
ifneq (1,$(RENODE_SHOW_GUI))
  RENODE_CONFIG_FLAGS += --disable-xwt
endif

# Configure local serial port
RENODE_SYSBUS_UART ?= sysbus.uart0
EMULATOR_SERIAL_PORT ?= /tmp/riot_$(APPLICATION)_$(BOARD)_uart
RENODE_CONFIG_FLAGS += -e "emulation CreateUartPtyTerminal \"term\" \"$(EMULATOR_SERIAL_PORT)\" true"
RENODE_CONFIG_FLAGS += -e "connector Connect $(RENODE_SYSBUS_UART) term"

# Set emulator variables
EMULATOR_FLAGS ?= $(RENODE_CONFIG_FLAGS) -e start
EMULATOR ?= $(RENODE)

# Configure the terminal
PORT = $(EMULATOR_SERIAL_PORT)
RIOT_TERMPROG := $(TERMPROG)
RIOT_TERMFLAGS := $(TERMFLAGS)
TERMPROG := $(RIOTTOOLS)/emulator/term.sh
TERMFLAGS := $(RIOT_EMULATOR) $(BOARD) $(APPDIR) $(RIOT_TERMPROG) '$(RIOT_TERMFLAGS)' $(EMULATOR_SERIAL_PORT)

# Configure the debugger
GDB_PORT ?= 3333
RENODE_DEBUG_FLAGS += $(RENODE_CONFIG_FLAGS)
RENODE_DEBUG_FLAGS += -e "machine StartGdbServer $(GDB_PORT) true"

DEBUGSERVER ?= $(EMULATOR)
DEBUGSERVER_FLAGS ?= $(RENODE_DEBUG_FLAGS)

DEBUGGER_FLAGS ?= $(BOARD) $(APPDIR) $(ELFFILE) $(GDB_PORT) "-ex \"monitor start\""
DEBUGGER ?= $(RIOTTOOLS)/emulator/debug.sh

# No flasher available with renode emulator
FLASHER ?=
FFLAGS ?=
