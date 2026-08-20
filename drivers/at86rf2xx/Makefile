
MODULE_DEFS_NEEDED += \
  at86rf212b \
  at86rf233 \
  at86rf2xx_aes_spi \
  ieee802154_security \
  netdev_ieee802154_oqpsk \
  netdev_ieee802154_rx_timestamp \
  openthread \
  sixlowpan \
  #
# exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out aes_spi.c,$(wildcard *.c))

# enable submodules
SUBMODULES := 1

include $(RIOTBASE)/Makefile.base
