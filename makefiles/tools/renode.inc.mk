RENODE ?= renode
RENODE_IMAGE_FILE ?= $(ELFFILE)
RENODE_BOARD_CONFIG ?= $(BOARDDIR)/dist/board.resc

# Use renode interactive commands to specify the image file and board config
RENODE_FLAGS += -e "set image_file '$(RENODE_IMAGE_FILE)'"
RENODE_FLAGS += -e "include @$(RENODE_BOARD_CONFIG)"

# Set emulator variables
EMULATOR_FLAGS ?= $(RENODE_FLAGS)
EMULATOR ?= $(RENODE)
