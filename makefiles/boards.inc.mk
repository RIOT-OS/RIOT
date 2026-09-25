# Default when RIOTBASE is not set and is executed from the RIOT directory
RIOTBOARD ?= $(or $(RIOTBASE),$(CURDIR))/boards
BOARDSDIRS ?= $(EXTERNAL_BOARD_DIRS) $(RIOTBOARD)

# Print error messages when these false friends have been set instead of
# `EXTERNAL_BOARD_DIRS`.
ifneq (,$(EXTERNAL_BOARDS_DIRS))
  $(error You hava set 'EXTERNAL_BOARDS_DIRS', did you mean 'EXTERNAL_BOARD_DIRS'?)
endif
ifneq (,$(EXTERNAL_BOARDS_DIR))
  $(error You have set 'EXTERNAL_BOARD_DIR', did you mean 'EXTERNAL_BOARD_DIRS'?)
endif
ifneq (,$(EXTERNAL_BOARD_DIR))
  $(error You have set 'EXTERNAL_BOARD_DIR', did you mean 'EXTERNAL_BOARD_DIRS'?)
endif

# List all boards in a directory
# By default, all directories in board directory except 'common'
#   use 'wildcard */.' to only list directories
_get_boards_in_directory = $(filter-out common,$(patsubst $1/%/.,%,$(wildcard $1/*/.)))

# Use `:=` so that it is evaluated before BOARDSDIRS gets eventually changed
ALLBOARDS := $(sort \
                    $(foreach dir,\
                              $(BOARDSDIRS),\
                              $(call _get_boards_in_directory,$(dir))))

# Set the default value from `BOARDS`
BOARDS ?= $(ALLBOARDS)

.PHONY: info-boards info-emulated-boards
info-boards:
	@echo $(BOARDS)

EMULATED_BOARDS_RENODE := $(wildcard $(BOARDSDIRS)/*/dist/board.resc)
EMULATED_BOARDS_QEMU := microbit
EMULATED_BOARDS_MGBA := gba_cartridge
EMULATED_BOARDS := \
  $(EMULATED_BOARDS_QEMU) \
  $(EMULATED_BOARDS_MGBA) \
  $(foreach board_path,$(EMULATED_BOARDS_RENODE),$(subst $(strip $(BOARDSDIRS)/),,$(subst /dist/board.resc,,$(board_path))))
  #

info-emulated-boards:
	@echo $(sort $(EMULATED_BOARDS))
