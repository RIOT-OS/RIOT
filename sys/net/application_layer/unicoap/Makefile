ifneq (,$(filter unicoap_driver_rfc7252_common_pdu,$(USEMODULE)))
  DIRS += drivers/rfc7252/common/pdu
endif
ifneq (,$(filter unicoap_driver_rfc7252_common_messaging,$(USEMODULE)))
  DIRS += drivers/rfc7252/common/messaging
endif
ifneq (,$(filter unicoap_driver_udp,$(USEMODULE)))
  DIRS += drivers/rfc7252/udp
endif
ifneq (,$(filter unicoap_driver_dtls,$(USEMODULE)))
  DIRS += drivers/rfc7252/dtls
endif

# MARK: unicoap_driver_extension_point

include $(RIOTBASE)/Makefile.base
