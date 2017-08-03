# exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out pm_fallback.c,$(wildcard *.c))

# enable submodules
SUBMODULES := 1

include $(RIOTBASE)/Makefile.base
