# Define the default board module.
BOARD_MODULE ?= slwrb4162a

# Find the header file that should exist if the module is supported.
MODULE_HEADER = $(wildcard $(RIOTBOARD)/slwstk6000b/modules/$(BOARD_MODULE)/include/board_module.h)

ifeq (,$(MODULE_HEADER))
  $(error Header file for $(BOARD_MODULE) is missing)
endif

# Lookup up CPU information using grep.
MODULE_INFO = $(shell grep $(BOARD_MODULE) $(RIOTBOARD)/slwstk6000b/modules.txt)

ifeq (,$(MODULE_INFO))
  $(error Unable to read module information for $(BOARD_MODULE))
endif

# Export variables to use in this build.
export MODULE_CPU = $(word 2, $(MODULE_INFO))
export MODULE_JLINK_DEVICE = $(word 3, $(MODULE_INFO))
