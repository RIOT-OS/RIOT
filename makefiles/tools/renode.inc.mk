RENODE ?= renode
RENODE_IMAGE_FILE ?= $(ELFFILE)
RENODE_BOARD_CONFIG ?= $(BOARDDIR)/dist/board.resc
DEBUGGER ?= gdb-multiarch
DEBUGGER_PORT ?= 3333

# Use renode interactive commands to specify the image file and board config
RENODE_CONFIG_FLAGS += -e "set image_file '$(RENODE_IMAGE_FILE)'"
RENODE_CONFIG_FLAGS += -e "include @$(RENODE_BOARD_CONFIG)"

# Set emulator variables
EMULATOR_FLAGS ?= $(RENODE_CONFIG_FLAGS) -e start
EMULATOR_DEBUG_FLAGS += $(RENODE_CONFIG_FLAGS)
EMULATOR_DEBUG_FLAGS += -e "machine StartGdbServer $(DEBUGGER_PORT) true"

EMULATOR ?= $(RENODE)
DEBUGGER_FLAGS ?= $(ELFFILE) -ex "target remote :$(DEBUGGER_PORT)" -ex "monitor start"
