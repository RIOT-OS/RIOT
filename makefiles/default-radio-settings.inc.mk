# Frequency Band (FBAND) variable can pass like: make BOARD=samr30-xpro FBAND=868 flash term
# with FBAND=868 changes channel/page setting only if transceiver is at86rf212b
ifeq (868, $(FBAND))
  # radio is at86rf212b sub GHz at FBAND=868MHz (channel = 0 / page = 0）
  ifneq (,$(filter at86rf212b,$(USEMODULE)))     
    FBAND_CHANNEL ?= 0
    CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(FBAND_CHANNEL)
    FBAND_PAGE ?= 0
    CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_PAGE=$(FBAND_PAGE)
  # if not using samr30-xpro radio is the same as default IEEE 802.15.4 2.4 GHz
  else                                        # radio is IEEE 802.15.4 2.4 GHz
    GHZ_CHANNEL ?= 26                                        
    CFLAGS += -DIEEE802154_DEFAULT_CHANNEL=$(GHZ_CHANNEL)
  endif
# Applying default setting if FBAND is 915MHz (Channel = 26 for 2.4 GHz, Channel = 5/Page = 2 for sub-GHz)
else                                          
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
endif

# Set a custom PAN ID if needed
ifneq (,$(DEFAULT_PAN_ID))
  CFLAGS += -DIEEE802154_DEFAULT_PANID=$(DEFAULT_PAN_ID)
endif
