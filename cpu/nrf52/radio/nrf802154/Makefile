MODULE = nrf802154

ifneq (,$(filter ieee802154_radio_hal,$(USEMODULE)))
  SRC += nrf802154_radio.c
else
  SRC += nrf802154.c
endif

include $(RIOTBASE)/Makefile.base
