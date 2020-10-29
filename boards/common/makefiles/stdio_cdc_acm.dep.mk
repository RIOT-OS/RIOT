ifeq (,$(filter-out stdio_cdc_acm,$(filter stdio_% slipdev_stdio,$(USEMODULE))))
  # Use stdio_cdc_acm only if no other stdio is requested explicitly.
  USEMODULE += stdio_cdc_acm
  FEATURES_REQUIRED += highlevel_stdio
endif
