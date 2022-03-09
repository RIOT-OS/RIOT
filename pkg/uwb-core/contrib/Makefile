MODULE = uwb-core_contrib

SRC = uwb_core.c

ifneq (,$(filter auto_init_uwb-core,$(USEMODULE)))
  SRC += uwb_core_init.c
endif

include $(RIOTBASE)/Makefile.base
