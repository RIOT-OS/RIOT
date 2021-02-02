MODULE = cc2538_rf

SRC = \
    cc2538_rf.c \
    cc2538_rf_getset.c \
    cc2538_rf_internal.c \
    #

# use netdev implementation when legacy is explicitly enabled
ifneq (,$(filter cc2538_rf_netdev_legacy,$(USEMODULE)))
  SRC += cc2538_rf_netdev.c
else
  SRC += cc2538_rf_radio_ops.c
endif

include $(RIOTBASE)/Makefile.base
