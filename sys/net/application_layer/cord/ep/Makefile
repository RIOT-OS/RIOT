MODULE = rdcli

SRC = rdcli.c

ifneq (,$(filter rdcli_standalone,$(USEMODULE)))
  SRC += rdcli_standalone.c
endif

include $(RIOTBASE)/Makefile.base
