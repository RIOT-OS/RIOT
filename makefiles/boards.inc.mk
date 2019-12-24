# Default when RIOTBASE is not set and is executed from the RIOT directory
BOARDSDIR ?= $(or $(RIOTBASE),$(CURDIR))/boards

# List all boards.
# By default, all directories in BOARDSDIR except 'common'
#   use 'wildcard */.' to only list directories
ALLBOARDS ?= $(sort $(filter-out common,$(patsubst $(BOARDSDIR)/%/.,%,$(wildcard $(BOARDSDIR)/*/.))))

# Set the default value from `BOARDS`
BOARDS ?= $(ALLBOARDS)

.PHONY: info-boards
info-boards:
	@echo $(BOARDS)
