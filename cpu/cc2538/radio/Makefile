MODULE = cc2538_rf

SRC = \
    cc2538_rf.c \
    cc2538_rf_getset.c \
    cc2538_rf_internal.c \
    #

ifneq (,$(filter ieee802154_radio_hal,$(USEMODULE)))
  SRC += cc2538_rf_radio_ops.c
else
  SRC += cc2538_rf_netdev.c
endif

include $(RIOTBASE)/Makefile.base
