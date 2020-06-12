# Frequency Band (FBAND) variable can pass like: make BOARD=samr30-xpro FBAND=868 flash term
# with FBAND=868 changes channel/page setting only if transceiver is at86rf212b
ifndef CONFIG_KCONFIG_MODULE_IEEE802154
  ifeq (868, $(FBAND))
    # radio is at86rf212b sub GHz at FBAND=868MHz (channel = 0 / page = 0)
    ifneq (,$(filter at86rf212b,$(USEMODULE)))
      CFLAGS += -DCONFIG_IEEE802154_DEFAULT_SUBGHZ_CHANNEL=0
      CFLAGS += -DCONFIG_IEEE802154_DEFAULT_SUBGHZ_PAGE=2
    # if not using samr30-xpro radio is the same as default IEEE 802.15.4 2.4 GHz
    else
    # radio is IEEE 802.15.4 2.4 GHz
      GHZ_CHANNEL ?= 26
      CFLAGS += -DCONFIG_IEEE802154_DEFAULT_CHANNEL=$(GHZ_CHANNEL)
    endif
  # Applying default setting if FBAND is 915MHz (Channel = 26 for 2.4 GHz, Channel = 5/Page = 2 for sub-GHz)
  else
    ifneq (,$(DEFAULT_CHANNEL))
      ifneq (,$(filter cc110x,$(USEMODULE)))# radio is cc110x sub-GHz
        CFLAGS += -DCONFIG_CC110X_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
      endif
    endif
  endif
  ifneq (,$(DEFAULT_PAN_ID))
    CFLAGS += -DCONFIG_IEEE802154_DEFAULT_PANID=$(DEFAULT_PAN_ID)
  endif
endif
