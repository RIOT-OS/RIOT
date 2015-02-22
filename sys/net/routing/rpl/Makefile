# RPL_MOP values by IANA https://tools.ietf.org/html/rfc6550#page-129
# RPL_MOP_NO_DOWNWARD_ROUTES  0x00
# RPL_MOP_NON_STORING_MODE    0x01
# RPL_MOP_STORING_MODE_NO_MC  0x02
# RPL_MOP_STORING_MODE_MC     0x03

# Set the RPL MOP iff it is matching a provided mode
MODE :=

ifeq (RPL_MOP_NO_DOWNWARD_ROUTES, $(RPL_MOP))
	MODE := 0x00
endif
ifeq (RPL_MOP_NON_STORING_MODE, $(RPL_MOP))
	MODE := 0x01
endif
ifeq (RPL_MOP_STORING_MODE_NO_MC, $(RPL_MOP))
	MODE := 0x02
endif
ifeq (RPL_MOP_STORING_MODE_MC, $(RPL_MOP))
	MODE := 0x03
endif

# If a valid RPL_MOP was provided, use it for RPL
ifneq (,$(MODE))
	CFLAGS += -DRPL_DEFAULT_MOP=$(MODE)
else
	# If no (valid) RPL_MOP has been provided, use the following parameters
	CFLAGS += -DRPL_DEFAULT_MOP=0x02
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
