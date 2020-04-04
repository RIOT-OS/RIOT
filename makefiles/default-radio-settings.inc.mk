# ISM vairable can pass like: make BOARD=samr30-xpro ISM=EU flash term
ifneq (EU,$(ISM))
  # Default setting for US ISM band (Channel 26 for 2.4 GHz, Channel 5/Page 2 for sub-GHz)
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
else      # Switching to ISM-EU if ISM=EU when make (sub-GHz channel = 0 / sub-GHz page = 0)
  ifneq (,$(filter cc110x,$(USEMODULE)))        # radio is cc110x sub-GHz
    CFLAGS += -DCC110X_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
  endif
  ifneq (,$(filter at86rf212b,$(USEMODULE)))    # radio sub-GHz channel = 0 / sub-GHz page = 0
    EU_CHANNEL ?= 0
    CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(EU_CHANNEL)
    EU_PAGE ?= 0
    CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_PAGE=$(EU_PAGE)
  else                                          # radio is IEEE 802.15.4 2.4 GHz
    CFLAGS += -DIEEE802154_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
  endif
endif

# Set a custom PAN ID if needed
ifneq (,$(DEFAULT_PAN_ID))
  CFLAGS += -DIEEE802154_DEFAULT_PANID=$(DEFAULT_PAN_ID)
endif
