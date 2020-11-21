RENODE ?= renode
RENODE_IMAGE_FILE ?= $(ELFFILE)
RENODE_BOARD_CONFIG ?= $(BOARDDIR)/dist/board.resc

# Use renode interactive commands to specify the image file and board config
RENODE_CONFIG_FLAGS += -e "set image_file '$(RENODE_IMAGE_FILE)'"
RENODE_CONFIG_FLAGS += -e "include @$(RENODE_BOARD_CONFIG)"

# Set emulator variables
EMULATOR_FLAGS ?= $(RENODE_CONFIG_FLAGS) -e start
EMULATOR ?= $(RENODE)

# Configure the debugger
GDB_PORT ?= 3333
EMULATOR_PIDFILE ?=
ifneq (,$(EMULATOR_PIDFILE))
  $(info Using Renode pid file $(EMULATOR_PIDFILE))
  RENODE_CONFIG_FLAGS += --pid-file $(EMULATOR_PIDFILE)
endif
RENODE_DEBUG_FLAGS += $(RENODE_CONFIG_FLAGS)
RENODE_DEBUG_FLAGS += -e "machine StartGdbServer $(GDB_PORT) true"

DEBUGSERVER ?= $(EMULATOR)
DEBUGSERVER_FLAGS ?= $(RENODE_DEBUG_FLAGS)

DEBUGGER_FLAGS ?= $(BOARD) $(APPDIR) $(ELFFILE) $(GDB_PORT) "-ex \"monitor start\""
DEBUGGER ?= $(RIOTTOOLS)/renode/renode-debug.sh
