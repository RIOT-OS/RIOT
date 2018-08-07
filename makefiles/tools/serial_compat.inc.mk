ifneq (,$(SERIAL))
  # The SERIAL setting is only available for backwards compatibility with older
  # settings.
  # Use DEBUG_ADAPTER_ID to specify the programmer serial number to use:
  # export DEBUG_ADAPTER_ID=<serial number>
  export DEBUG_ADAPTER_ID ?= $(SERIAL)
endif
