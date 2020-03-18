STDIO_MODULES = \
  slipdev_stdio \
  stdio_cdc_acm \
  stdio_ethos \
  stdio_null \
  stdio_rtt \
  stdio_uart \
  #

# Since USEMODULE and DEFAULT_MODULEs are recursively expanded we need to
# disable DEFAULT_MODULEs before any of there dependencies are evaluated,
# otherwise the disabled MODULE will be in USEMODULE (triggering) its
# dependencies, and then removed but leaving its dependencies
ifneq (,$(filter $(filter-out stdio_rtt,$(STDIO_MODULES)),$(USEMODULE)))
  # stdio_rtt cannot be used when another STDIO is loaded
  DISABLE_MODULE += stdio_rtt
endif

ifneq (,$(filter $(filter-out stdio_cdc_acm,$(STDIO_MODULES)),$(USEMODULE)))
  # stdio_cdc_acm cannot be used when another STDIO is loaded
  DISABLE_MODULE += stdio_cdc_acm
endif

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
  # The arduino bootloader feature cannot be used if the stdio_cdc_acm module
  # is not used
  FEATURES_BLACKLIST += bootloader_arduino
endif
