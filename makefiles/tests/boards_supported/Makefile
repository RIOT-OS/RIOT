# In order to be able to include info-global.inc.mk, we need to provide some variables
RIOTBASE            ?= $(abspath $(CURDIR)/../../..)
RIOTBOARD           ?= $(RIOTBASE)/boards
RIOTMAKE            ?= $(RIOTBASE)/makefiles
RIOTCPU             ?= $(RIOTBASE)/cpu
RIOTTOOLS           ?= $(RIOTBASE)/dist/tools
LAST_MAKEFILEDIR    = $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

# Folders to search: First the external boards, than the official
BOARDSDIRS := $(EXTERNAL_BOARD_DIRS) $(RIOTBOARD)

# Take the first folder in $(BOARDSDIRS) that contains a folder named $(BOARD)
BOARDDIR := $(word 1,$(foreach dir,$(BOARDSDIRS),$(wildcard $(dir)/$(BOARD)/.)))
# Sanitize folder
BOARDDIR := $(abspath $(BOARDDIR))

include $(RIOTMAKE)/utils/strings.mk
include $(RIOTMAKE)/boards.inc.mk
include $(RIOTMAKE)/info-global.inc.mk

ifneq (,$(BOARDS_WITH_MISSING_FEATURES))
  $(info BOARDS_FEATURES_MISSING=$(BOARDS_FEATURES_MISSING))
  $(error The CI will never build for the following boards: $(BOARDS_WITH_MISSING_FEATURES))
endif

.PHONY: all
all:
	@echo "Success"
