MODULE = tlv

SUBMODULES := 1

# exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out simple.c ber.c writer.c,$(wildcard *.c))

include $(RIOTBASE)/Makefile.base
