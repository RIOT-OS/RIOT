ifeq (,$(filter stdio_% slipdev_stdio,$(USEMODULE)))
  # Use stdio_tinyusb_cdc_acm only if no other stdio is requested explicitly.
  # and usbus is used for any other reason
  ifneq (,$(filter usbus,$(USEMODULE)))
    USEMODULE += stdio_cdc_acm
  else
    USEMODULE += stdio_tinyusb_cdc_acm
  endif

  FEATURES_REQUIRED += highlevel_stdio
endif
