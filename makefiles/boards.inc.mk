# Default when RIOTBASE is not set and is executed from the RIOT directory
BOARDSDIR ?= $(or $(RIOTBASE),$(CURDIR))/boards

# List all boards in a directory
# By default, all directories in BOARDSDIR except 'common'
#   use 'wildcard */.' to only list directories
_get_boards_in_directory = $(filter-out common,$(patsubst $1/%/.,%,$(wildcard $1/*/.)))

# If BOARDSDIR is not RIOTBOARD also list BOARDS in RIOTBOAR
ifneq ($(RIOTBOARD),$(BOARDSDIR))
  ALLBOARDS_RIOTBOARD ?= $(call _get_boards_in_directory,$(RIOTBOARD))
endif

# Get all boards
ALLBOARDS ?= $(sort $(call _get_boards_in_directory,$(BOARDSDIR)) $(ALLBOARDS_RIOTBOARD))

# Set the default value from `BOARDS`
BOARDS ?= $(ALLBOARDS)

.PHONY: info-boards
info-boards:
	@echo $(BOARDS)
