# Default when RIOTBASE is not set and is executed from the RIOT directory
RIOTBOARD ?= $(or $(RIOTBASE),$(CURDIR))/boards

# List all boards.
# By default, all directories in RIOTBOARD except 'common'
#   use 'wildcard */.' to only list directories
ALLBOARDS ?= $(sort $(filter-out common,$(patsubst $(RIOTBOARD)/%/.,%,$(wildcard $(RIOTBOARD)/*/.))))

# Set the default value from `BOARDS`
BOARDS ?= $(ALLBOARDS)

.PHONY: info-boards
info-boards:
	@echo $(BOARDS)
