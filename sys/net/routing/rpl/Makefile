MODE = $(shell grep "RPL_DEFAULT_MOP RPL_NON_STORING_MODE" ../../include/rpl/rpl_config.h)
ifneq (,$(MODE))
	DIRS += rpl_nonstoring
else
	DIRS += rpl_storing
endif

# Set the maximum number of routing entries to 128 if no number is provided
ifneq (,$(RPL_MAX_ROUTING_ENTRIES))
	CFLAGS += -DRPL_MAX_ROUTING_ENTRIES=$(RPL_MAX_ROUTING_ENTRIES)
endif

# Define this node as root at compile time (required only for non-storing mode) 
ifneq (,$(RPL_NODE_IS_ROOT))
	CFLAGS += -DRPL_NODE_IS_ROOT
endif

include $(RIOTBASE)/Makefile.base
