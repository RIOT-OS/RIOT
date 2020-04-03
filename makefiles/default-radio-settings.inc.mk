# ISM = 868Mhz Channel & Page Switch for EU area
ifeq (868,$(ISM))
  ifneq (,$(filter at86rf212b,$(USEMODULE)))    # radio is IEEE 802.15.4 sub-GHz
    EU_CHANNEL ?= 0
    CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(UK_CHANNEL)
    EU_PAGE ?= 0
    CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_PAGE=$(UK_PAGE)
    # As Issues/PRS reference #13526 has already sloved, we don't need change the SPI_CLK anaymore
    # SCLK := SPI_CLK_1MHZ
    # CFLAGS += -DAT86RF2XX_PARAM_SPI_CLK=$(SCLK)
  endif
endif
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
