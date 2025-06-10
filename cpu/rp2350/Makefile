MODULE = cpu

USEPKG += picosdk

DIRS = $(RIOTCPU)/cortexm_common periph

include $(RIOTBASE)/Makefile.base

# Check if picosdk directory exists, otherwise build it
ifeq (,$(wildcard $(RIOTBASE)/dist/tools/picosdk/picosdk))
  $(info Building picosdk...)
  $(shell $(MAKE) -C $(RIOTBASE)/dist/tools/picosdk)
endif
