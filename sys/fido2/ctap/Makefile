MODULE := fido2_ctap

MODULE_DEFS_NEEDED += \
  fido2_ctap_transport_hid \
  #

ifneq (,$(filter fido2_ctap_%,$(USEMODULE)))
  DIRS += transport
endif

include $(RIOTBASE)/Makefile.base
