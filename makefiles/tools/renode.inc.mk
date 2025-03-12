RENODE ?= renode
RENODE_IMAGE_FILE ?= $(EXECUTABLE)
RENODE_BOARD_CONFIG ?= $(BOARDDIR)/dist/board.resc

# Global build system configurations
FLASHFILE ?= $(EXECUTABLE)
EMULATORDEPS += $(RENODE_BOARD_CONFIG)

ifeq (,$(EMULATOR_TMP_DIR))
  EMULATOR_TMP_DIR := $(shell mktemp -td riot_$(APPLICATION)_$(BOARD).XXXXX)
endif

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

# Configure renode telnet port to allow sending command to the monitor while
# the emulator is running (used to send reset command)
RENODE_TELNET_PORT ?= 1234
RENODE_CONFIG_FLAGS += -P $(RENODE_TELNET_PORT)

# Renode GUI
RENODE_SHOW_GUI ?= 0
ifneq (1,$(RENODE_SHOW_GUI))
  RENODE_CONFIG_FLAGS += --disable-xwt
endif

# Configure local serial port
RENODE_SYSBUS_UART ?= sysbus.uart0
EMULATOR_SERIAL_PORT ?= $(EMULATOR_TMP_DIR)/uart
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
TERMFLAGS := $(RIOT_EMULATOR) $(BOARD) $(APPDIR) $(RIOT_TERMPROG) '$(RIOT_TERMFLAGS)' $(EMULATOR_SERIAL_PORT) $(EMULATOR_TMP_DIR)

# Configure the debugger
GDB_PORT ?= 3333
GDB_REMOTE ?= :$(GDB_PORT)
RENODE_DEBUG_FLAGS += $(RENODE_CONFIG_FLAGS)
RENODE_DEBUG_FLAGS += -e "machine StartGdbServer $(GDB_PORT) true"

DEBUGSERVER ?= $(EMULATOR)
DEBUGSERVER_FLAGS ?= $(RENODE_DEBUG_FLAGS)

DEBUGGER_FLAGS ?= $(BOARD) $(APPDIR) $(DEBUG_EXECUTABLE) $(GDB_REMOTE) $(EMULATOR_TMP_DIR) "-ex \"monitor start\""
DEBUGGER ?= $(RIOTTOOLS)/emulator/debug.sh

RESET ?= bash
RESET_FLAGS ?= -c "{ sleep 0.2;echo machine RequestReset; } | telnet localhost $(RENODE_TELNET_PORT)" || true

# No flasher available with renode emulator
FLASHER ?=
FFLAGS ?=
