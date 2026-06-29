# list of available stdio backends
STDIO_MODULES = \
  stdio_usb_serial_jtag \
  stdio_udp \
  stdio_uart \
  stdio_tinyusb_cdc_acm \
  stdio_telnet \
  stdio_slipdev \
  stdio_semihosting \
  stdio_rtt \
  stdio_null \
  stdio_nimble \
  stdio_native \
  stdio_fb \
  stdio_ethos \
  stdio_cdc_acm \
  #

# legacy module aliases for stdio backends
STDIO_LEGACY_MODULES = \
  ethos_stdio \
  stdio_ethos \
  #
