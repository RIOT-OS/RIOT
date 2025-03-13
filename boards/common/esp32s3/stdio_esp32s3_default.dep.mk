ifneq (,$(filter stdio_default,$(USEMODULE)))
  ifneq (,$(filter usbus,$(USEMODULE)))
    USEMODULE += stdio_cdc_acm
    FEATURES_REQUIRED += highlevel_stdio
  else ifneq (,$(filter tinyusb_device,$(USEMODULE)))
    USEMODULE += stdio_tinyusb_cdc_acm
    FEATURES_REQUIRED += highlevel_stdio
  else
    # Use stdio_usb_serial_jtag if no other stdio is requested explicitly
    # and neither USBUS nor tinyusb_device are used
    USEMODULE += stdio_usb_serial_jtag
  endif
  # Even if only stdio_usb_serial_jtag is enabled, usb_board_reset is enabled
  # since there should be a CDC ACM interface in any case. This is necessary,
  # for example, to reset the board if stdio_cdc_acm or stdio_tinyusb_cdc_acm
  # was previously used.
  USEMODULE += usb_board_reset
  include $(RIOTMAKE)/tools/usb_board_reset.mk
endif
