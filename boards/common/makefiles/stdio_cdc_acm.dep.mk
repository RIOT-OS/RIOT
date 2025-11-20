ifneq (,$(filter stdio_default,$(USEMODULE)))
  ifneq (,$(filter tinyusb_device,$(USEMODULE))$(filter tinyusb,$(USEPKG)))
    # Use stdio_tinyusb_cdc_acm only if no other stdio is requested explicitly
    # and tinyusb_device is used for any other reason
    USEMODULE += stdio_tinyusb_cdc_acm
  else
    # Use stdio_cdc_acm only if no other stdio is requested explicitly.
    USEMODULE += stdio_cdc_acm
  endif
  FEATURES_REQUIRED += highlevel_stdio

  # Enforce tests to wait a bit for the serial port after reset
  TERM_DELAY ?= 2
  TESTRUNNER_CONNECT_DELAY ?= $(TERM_DELAY)
  $(call target-export-variables,test,TESTRUNNER_CONNECT_DELAY)
endif
