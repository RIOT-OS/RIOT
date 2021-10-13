STDIO_MODULES = \
  slipdev_stdio \
  stdio_cdc_acm \
  stdio_ethos \
  stdio_null \
  stdio_rtt \
  stdio_semihosting \
  stdio_uart \
  #

ifneq (,$(filter newlib picolibc,$(USEMODULE)))
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
  FEATURES_REQUIRED_ANY += periph_uart|periph_lpuart
endif

ifneq (,$(filter stdio_semihosting,$(USEMODULE)))
  USEMODULE += xtimer
  FEATURES_REQUIRED_ANY += cpu_core_cortexm|arch_riscv
endif

# enable stdout buffering for modules that benefit from sending out buffers in larger chunks
ifneq (,$(filter picolibc,$(USEMODULE)))
  ifneq (,$(filter stdio_cdc_acm stdio_ethos slipdev_stdio stdio_semihosting,$(USEMODULE)))
    USEMODULE += picolibc_stdout_buffered
  endif
endif
