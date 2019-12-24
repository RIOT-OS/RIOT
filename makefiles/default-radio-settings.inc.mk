# Set a custom channel if needed
ifneq (,$(DEFAULT_CHANNEL))
  ifneq (,$(filter cc110x,$(USEMODULE)))        # radio is cc110x sub-GHz
    CFLAGS += -DCC110X_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
  endif

  ifneq (,$(filter at86rf212b,$(USEMODULE)))    # radio is IEEE 802.15.4 sub-GHz
    CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(DEFAULT_CHANNEL)
  else                                          # radio is IEEE 802.15.4 2.4 GHz
    CFLAGS += -DIEEE802154_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
  endif
endif

# Set a custom PAN ID if needed
ifneq (,$(DEFAULT_PAN_ID))
  CFLAGS += -DIEEE802154_DEFAULT_PANID=$(DEFAULT_PAN_ID)
endif
