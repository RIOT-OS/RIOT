
MODULE_DEFS_NEEDED += \
  ethos_stdio \
  #
# exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out stdio.c,$(wildcard *.c))

# enable submodules
SUBMODULES := 1

include $(RIOTBASE)/Makefile.base
