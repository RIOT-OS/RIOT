MODULE := fido2_ctap

ifneq (,$(filter fido2_ctap_%,$(USEMODULE)))
  DIRS += transport
endif

ifneq (,$(filter fido2_ctap_hid,$(USEMODULE)))
	CFLAGS += -DCONFIG_CTAP_HID=1
endif

include $(RIOTBASE)/Makefile.base
