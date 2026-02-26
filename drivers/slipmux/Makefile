# exclude submodule sources from *.c wildcard source selection
#SRC := $(filter-out stdio.c,$(wildcard *.c))
SRC := slipdev.c auto_init.c

ifneq (,$(filter slipdev_stdio,$(USEMODULE)))
	SRC += stdio.c
endif

# enable submodules
#SUBMODULES := 1

include $(RIOTBASE)/Makefile.base
