# exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out aes_spi.c,$(wildcard *.c))

# enable submodules
SUBMODULES := 1

include $(RIOTBASE)/Makefile.base
