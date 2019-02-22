MODULE = nimble_host

# xxx: the following files do not compile for RIOT, so we skip them
IGNORE := ble_gatts_lcl.c

SRC := $(filter-out $(IGNORE),$(wildcard *.c))

include $(RIOTBASE)/Makefile.base
