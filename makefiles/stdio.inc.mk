STDIO_MODULES = \
  slipdev_stdio \
  stdio_cdc_acm \
  stdio_ethos \
  stdio_nimble \
  stdio_null \
  stdio_rtt \
  stdio_semihosting \
  stdio_uart \
  stdio_telnet \
  #

ifneq (,$(filter newlib picolibc,$(USEMODULE)))
  ifeq (,$(filter $(STDIO_MODULES),$(USEMODULE)))
    USEMODULE += stdio_uart
  endif
endif

ifneq (,$(filter stdio_cdc_acm,$(USEMODULE)))
  USEMODULE += usbus_cdc_acm
  USEMODULE += isrpipe
  USEMODULE += stdio_available
endif

ifneq (,$(filter stdio_rtt,$(USEMODULE)))
  USEMODULE += ztimer_msec
endif

ifneq (,$(filter stdio_ethos,$(USEMODULE)))
  USEMODULE += ethos_stdio
  USEMODULE += stdin
endif

ifneq (,$(filter stdio_nimble,$(USEMODULE)))
  USEMODULE += stdio_available
  USEPKG += nimble
  USEMODULE += tsrb
  USEMODULE += isrpipe
  USEMODULE += nimble_svc_gap
  USEMODULE += nimble_svc_gatt
  ifneq (,$(filter stdio_nimble_debug,$(USEMODULE)))
    FEATURES_REQUIRED += periph_uart
  endif
endif

ifneq (,$(filter stdin,$(USEMODULE)))
  ifneq (,$(filter stdio_uart,$(USEMODULE)))
    USEMODULE += stdio_uart_rx
  endif
endif

ifneq (,$(filter stdio_uart_rx,$(USEMODULE)))
  USEMODULE += isrpipe
  USEMODULE += stdio_uart
  USEMODULE += stdio_available
endif

ifneq (,$(filter stdio_uart,$(USEMODULE)))
  FEATURES_REQUIRED_ANY += periph_uart|periph_lpuart
endif

ifneq (,$(filter stdio_semihosting,$(USEMODULE)))
  USEMODULE += ztimer_msec
  FEATURES_REQUIRED_ANY += cpu_core_cortexm|arch_riscv
endif

ifneq (,$(filter stdio_telnet,$(USEMODULE)))
  DEFAULT_MODULE += auto_init_telnet
  USEMODULE += telnet
endif

# enable stdout buffering for modules that benefit from sending out buffers in larger chunks
ifneq (,$(filter picolibc,$(USEMODULE)))
  ifneq (,$(filter stdio_cdc_acm stdio_ethos slipdev_stdio stdio_semihosting,$(USEMODULE)))
    USEMODULE += picolibc_stdout_buffered
  endif
endif
