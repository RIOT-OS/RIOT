MODULE = openwsn

MODULE_DEFS_NEEDED += \
  at86rf2xx \
  cc2538_rf \
  nrf802154 \
  openwsn_debugpins \
  openwsn_leds \
  openwsn_radio_hal \
  openwsn_radio_netdev \
  openwsn_sctimer \
  openwsn_serial \
  pm_layered \
  ztimer_usec \
  #

SRC := $(filter-out sctimer_% radio_%,$(wildcard *.c))

ifneq (,$(filter openwsn_sctimer_rtt,$(USEMODULE)))
  SRC += sctimer_rtt.c
endif

ifneq (,$(filter openwsn_sctimer_ztimer,$(USEMODULE)))
  SRC += sctimer_ztimer.c
endif

ifneq (,$(filter openwsn_radio_hal,$(USEMODULE)))
  SRC += radio_hal.c
endif

ifneq (,$(filter openwsn_radio_netdev,$(USEMODULE)))
  SRC += radio_netdev.c
endif

include $(RIOTBASE)/Makefile.base
