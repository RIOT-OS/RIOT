# exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out mbox.c msg.c thread_flags.c priority_inheritance.c,$(wildcard *.c))

# enable submodules
SUBMODULES := 1
SUBMODULES_NOFORCE := 1

include $(RIOTBASE)/Makefile.base
