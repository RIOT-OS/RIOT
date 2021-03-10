MODULE = nrf802154

# use netdev implementation when legacy is explicitly enabled
ifneq (,$(filter nrf802154_netdev_legacy,$(USEMODULE)))
  SRC += nrf802154.c
else
  SRC += nrf802154_radio.c
endif

include $(RIOTBASE)/Makefile.base
