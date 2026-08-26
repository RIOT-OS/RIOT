SRC := at86rf215.c at86rf215_getset.c at86rf215_radio_ops.c at86rf215_internal.c

ifneq (,$(filter ieee802154_phy_mr_ofdm,$(USEMODULE)))
  SRC += at86rf215_ofdm.c
endif
ifneq (,$(filter ieee802154_phy_mr_fsk,$(USEMODULE)))
  SRC += at86rf215_fsk.c
endif
ifneq (,$(filter ieee802154_phy_oqpsk ieee802154_phy_mr_oqpsk,$(USEMODULE)))
  SRC += at86rf215_o-qpsk.c
endif

include $(RIOTBASE)/Makefile.base
