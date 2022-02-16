SRC := at86rf215.c at86rf215_getset.c at86rf215_netdev.c at86rf215_internal.c

ifneq (,$(filter netdev_ieee802154_mr_ofdm,$(USEMODULE)))
  SRC += at86rf215_ofdm.c
endif
ifneq (,$(filter netdev_ieee802154_mr_fsk,$(USEMODULE)))
  SRC += at86rf215_fsk.c
endif
ifneq (,$(filter netdev_ieee802154_oqpsk netdev_ieee802154_mr_oqpsk,$(USEMODULE)))
  SRC += at86rf215_o-qpsk.c
endif

include $(RIOTBASE)/Makefile.base
