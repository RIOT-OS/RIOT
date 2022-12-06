ifeq (,$(filter-out stdio_cdc_acm,$(filter stdio_% slipdev_stdio,$(USEMODULE))))
  ifneq (,$(filter tinyusb_device,$(USEMODULE)))
    # Use stdio_tinyusb_cdc_acm only if no other stdio is requested explicitly
    # and tinyusb_device is used for any other reason
    USEMODULE += stdio_tinyusb_cdc_acm
  else
    # Use stdio_cdc_acm only if no other stdio is requested explicitly.
    USEMODULE += stdio_cdc_acm
  endif
  FEATURES_REQUIRED += highlevel_stdio
endif
