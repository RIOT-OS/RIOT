include $(RIOTMAKE)/utils/strings.mk

# List of available stdio backend modules.
STDIO_MODULES = \
  stdio_cdc_acm \
  stdio_ethos \
  stdio_fb \
  stdio_native \
  stdio_nimble \
  stdio_null \
  stdio_rtt \
  stdio_semihosting \
  stdio_slipdev \
  stdio_telnet \
  stdio_tinyusb_cdc_acm \
  stdio_uart \
  stdio_udp \
  stdio_usb_serial_jtag \
  #

# List of available legacy stdio backend modules.
STDIO_LEGACY_MODULES = \
  ethos_stdio \
  stdio_ethos \
  #

ifeq (,$(filter $(STDIO_LEGACY_MODULES),$(USEMODULE)))
  USEMODULE += stdio
endif

ifneq (,$(filter stdin,$(USEMODULE)))
  USEMODULE += isrpipe
endif

# Check if more than one STDIO has been selected, if so add stdio_dispatch.
# This check will fail if 10 or more modules are selected.
ifeq (1, $(call _is_greater,$(words $(sort $(filter $(STDIO_MODULES),$(USEMODULE)))),1))
  USEMODULE += stdio_dispatch
endif

ifneq (,$(filter stdio_cdc_acm,$(USEMODULE)))
  USEMODULE += usbus_cdc_acm
  USEMODULE += isrpipe
  USEMODULE += stdio_available
endif

ifneq (,$(filter stdio_tinyusb_cdc_acm,$(USEMODULE)))
  USEPKG += tinyusb
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

ifneq (,$(filter stdio_udp,$(USEMODULE)))
  USEMODULE += sock_udp
  USEMODULE += sock_async
endif

# Enable stdout buffering for modules that benefit from sending out buffers in
# larger chunks.
ifneq (,$(filter picolibc,$(USEMODULE)))
  ifneq (,$(filter stdio_cdc_acm stdio_ethos stdio_slipdev stdio_semihosting stdio_tinyusb_cdc_acm,$(USEMODULE)))
    USEMODULE += picolibc_stdout_buffered
  endif
endif

# Select stdio_default if no other STDIO backend has been selected so far.
# If no STDIO backend has been selected in the second round of dependency
# resolution, then select stdio_uart.
ifeq (,$(filter $(STDIO_MODULES),$(USEMODULE)))
  ifneq (,$(filter stdio_default,$(USEMODULE)))
    USEMODULE += stdio_uart
  endif
  USEMODULE += stdio_default
endif
