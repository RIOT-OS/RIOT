SRC := nanocoap.c

ifneq (,$(filter nanocoap_sock,$(USEMODULE)))
  SRC += sock.c
endif

ifneq (,$(filter nanocoap_opt2,$(USEMODULE)))
  SRC += opt2_add.c
endif

include $(RIOTBASE)/Makefile.base
