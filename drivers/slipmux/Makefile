# exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out stdio.c coap.c net.c,$(wildcard *.c))

# enable submodules
SUBMODULES := 1

include $(RIOTBASE)/Makefile.base
