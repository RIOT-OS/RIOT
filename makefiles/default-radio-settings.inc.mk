# Set a custom channel if needed
ifndef CONFIG_KCONFIG_MODULE_IEEE802154
  ifneq (,$(DEFAULT_CHANNEL))
    ifneq (,$(filter cc110x,$(USEMODULE)))        # radio is cc110x sub-GHz
      CFLAGS += -DCONFIG_CC110X_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
    endif

    ifneq (,$(filter at86rf212b,$(USEMODULE)))    # radio is IEEE 802.15.4 sub-GHz
      CFLAGS += -DCONFIG_IEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(DEFAULT_CHANNEL)
    else                                          # radio is IEEE 802.15.4 2.4 GHz
      CFLAGS += -DCONFIG_IEEE802154_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
    endif
  endif

  # Set a custom PAN ID if needed
  ifneq (,$(DEFAULT_PAN_ID))
    CFLAGS += -DCONFIG_IEEE802154_DEFAULT_PANID=$(DEFAULT_PAN_ID)
  endif
endif
