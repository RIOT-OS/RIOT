APPLICATION = pkg_lmic

include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi periph_gpio

USEMODULE += xtimer
USEPKG += lmic

# Can be either sx1276 or sx1272
LMIC_DRIVER ?= sx1276

# Can be either eu868 or us915
LMIC_ISM_BAND ?= eu868

# Can be either OTAA or ABP
LMIC_JOIN_PROCEDURE ?= ABP

CFLAGS += -DLMIC_DEBUG_LEVEL=0
CFLAGS += -DOSTICKS_PER_SEC=62500UL

ifeq ($(LMIC_DRIVER), sx1272)
  CFLAGS += -DCFG_sx1272_radio
else
  # SX1276
  CFLAGS += -DCFG_sx1276_radio
endif

ifeq ($(LMIC_ISM_BAND), us915)
  CFLAGS += -DCFG_us915
else
  # EU868
  CFLAGS += -DCFG_eu868
endif

ifeq ($(LMIC_JOIN_PROCEDURE), ABP)
  CFLAGS += -DLMIC_JOIN_ABP
else
  # OTAA
  CFLAGS += -DLMIC_JOIN_OTAA
endif

include $(RIOTBASE)/Makefile.include
