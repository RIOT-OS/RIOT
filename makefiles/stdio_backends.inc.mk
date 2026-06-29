# list of available stdio backends
STDIO_MODULES = \
  stdio_cdc_acm \
  stdio_ethos \
  stdio_native \
  stdio_nimble \
  stdio_null \
  stdio_rtt \
  stdio_semihosting \
  stdio_slipdev \
  stdio_uart \
  stdio_udp \
  stdio_telnet \
  stdio_tinyusb_cdc_acm \
  stdio_usb_serial_jtag \
  stdio_fb \
  #

# legacy module aliases for stdio backends
STDIO_LEGACY_MODULES = \
  ethos_stdio \
  stdio_ethos \
  #
