
MODULE_DEFS_NEEDED += \
  dns \
  ipv4_addr \
  ipv6_addr \
  unicoap_driver_dtls \
  unicoap_driver_rfc7252_common \
  unicoap_driver_slipmux \
  unicoap_driver_udp \
  unicoap_server \
  unicoap_server_resource_declarations \
  unicoap_sock_support \
  #
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
ifneq (,$(filter unicoap_driver_slipmux,$(USEMODULE)))
  DIRS += drivers/rfc7252/slipmux
endif

# MARK: unicoap_driver_extension_point

ifneq (,$(filter unicoap_server,$(USEMODULE)))
  DIRS += server
endif

include $(RIOTBASE)/Makefile.base
