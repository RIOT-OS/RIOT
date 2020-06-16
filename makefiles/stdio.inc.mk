STDIO_MODULES = \
  slipdev_stdio \
  stdio_cdc_acm \
  stdio_ethos \
  stdio_null \
  stdio_rtt \
  stdio_semihosting \
  stdio_uart \
  #

ifneq (,$(filter newlib,$(USEMODULE)))
  ifeq (,$(filter $(STDIO_MODULES),$(USEMODULE)))
    USEMODULE += stdio_uart
  endif
endif

ifneq (,$(filter stdio_cdc_acm,$(USEMODULE)))
  USEMODULE += usbus_cdc_acm
  USEMODULE += isrpipe
endif

ifneq (,$(filter stdio_rtt,$(USEMODULE)))
  USEMODULE += xtimer
endif

ifneq (,$(filter stdio_ethos,$(USEMODULE)))
  USEMODULE += ethos
  USEMODULE += stdin
  USEMODULE += stdio_uart
endif

ifneq (,$(filter stdin,$(USEMODULE)))
  ifneq (,$(filter stdio_uart,$(USEMODULE)))
    USEMODULE += stdio_uart_rx
  endif
endif

ifneq (,$(filter stdio_uart_rx,$(USEMODULE)))
  USEMODULE += isrpipe
  USEMODULE += stdio_uart
endif

ifneq (,$(filter stdio_uart,$(USEMODULE)))
  FEATURES_REQUIRED += periph_uart
endif

ifeq (,$(filter stdio_cdc_acm,$(USEMODULE)))
  # The arduino and nrfutil bootloader features cannot be used if the
  # stdio_cdc_acm module is not used
  FEATURES_BLACKLIST += bootloader_arduino
  FEATURES_BLACKLIST += bootloader_nrfutil
endif

ifneq (,$(filter stdio_semihosting,$(USEMODULE)))
  USEMODULE += xtimer
  FEATURES_REQUIRED += cpu_core_cortexm
endif
