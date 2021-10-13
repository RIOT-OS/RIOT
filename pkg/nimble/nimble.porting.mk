ifneq (,$(filter mynewt-core,$(USEMODULE)))
  SRC = nimble_port.c
endif

include $(RIOTBASE)/Makefile.base
