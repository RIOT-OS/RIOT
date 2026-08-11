MODULE := fido2_ctap_transport

MODULE_DEFS_NEEDED += \
  fido2_ctap_transport_hid \
  #

ifneq (,$(filter fido2_ctap_transport_hid,$(USEMODULE)))
	DIRS += hid
endif

include $(RIOTBASE)/Makefile.base
