MODULE = mjson

# The mjson_merge function requires an alloca implementation, disabled to
# prevent stack memory allocations
CFLAGS += -DMJSON_ENABLE_MERGE=0

include $(RIOTBASE)/Makefile.base
