# Include this file to use the common buildout directory.

BUILD_DIR      ?= $(RIOTBASE)/build
LOGS_DIR       ?= $(BUILD_DIR)/logs

override BUILD_DIR      := $(abspath $(BUILD_DIR))
override LOGS_DIR       := $(abspath $(LOGS_DIR))

# This rule is OK if the buildout dir only contains directorties (which should
# be the case for tidyness)

$(BUILD_DIR)/%:
	$(Q)mkdir -p $@
