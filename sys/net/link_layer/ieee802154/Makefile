MODULE = ieee802154

MODULE_DEFS_NEEDED += \
  netdev_ieee802154_mr_fsk \
  netdev_ieee802154_mr_ofdm \
  netdev_ieee802154_mr_oqpsk \
  #

SRC = \
    ieee802154.c \
    #

ifneq (,$(filter ieee802154_security,$(USEMODULE)))
	SRC += security.c
endif

ifneq (,$(filter ieee802154_submac,$(USEMODULE)))
	SRC += submac.c
endif

include $(RIOTBASE)/Makefile.base
