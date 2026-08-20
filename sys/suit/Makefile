
MODULE_DEFS_NEEDED += \
  nanocoap_dtls \
  progress_bar \
  suit_transport_coap \
  suit_transport_mock \
  suit_transport_vfs \
  #
ifneq (,$(filter suit_transport_%,$(USEMODULE)))
  DIRS += transport
endif

ifneq (,$(filter suit_storage_%,$(USEMODULE)))
  DIRS += storage
endif

include $(RIOTBASE)/Makefile.base
