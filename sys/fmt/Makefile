
MODULE_DEFS_NEEDED += \
  stdio_native \
  #
# exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out table.c,$(wildcard *.c))

# enable submodules
SUBMODULES := 1

include $(RIOTBASE)/Makefile.base
