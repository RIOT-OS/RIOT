# exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out mbox.c msg.c msg_bus.c thread.c thread_flags.c thread_flags_group.c,$(wildcard *.c))

# enable submodules
SUBMODULES := 1

# this module is expected to pass static analysis
MODULE_SUPPORTS_STATIC_ANALYSIS := 1

include $(RIOTBASE)/Makefile.base
