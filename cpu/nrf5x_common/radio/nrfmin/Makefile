MODULE = nrfmin

SRC = nrfmin.c

ifneq (,$(filter gnrc_netif,$(USEMODULE)))
  SRC += nrfmin_gnrc.c
endif

include $(RIOTBASE)/Makefile.base
