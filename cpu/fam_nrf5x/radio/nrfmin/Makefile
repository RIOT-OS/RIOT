MODULE = nrfmin

SRC = nrfmin.c

ifneq (,$(filter gnrc_netdev_default,$(USEMODULE)))
  SRC += nrfmin_gnrc.c
endif

include $(RIOTBASE)/Makefile.base
