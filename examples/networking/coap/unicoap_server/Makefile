# Default Makefile, for unicoap message APIs

# name of your application
APPLICATION = unicoap_server

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default

# use GNRC by default
LWIP_IPV4 ?= 0
LWIP_IPV6 ?= 0

ifeq (,$(filter 1, $(LWIP_IPV4) $(LWIP_IPV6)))
  USEMODULE += auto_init_gnrc_netif
  # Specify the mandatory networking modules
  USEMODULE += gnrc_ipv6_default
  # Additional networking modules that can be dropped if not needed
  USEMODULE += gnrc_icmpv6_echo
else
  ifeq (1,$(LWIP_IPV4))
    USEMODULE += ipv4_addr

    USEMODULE += lwip_arp
    USEMODULE += lwip_ipv4
    USEMODULE += lwip_dhcp_auto
    CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1
  endif

  ifeq (1,$(LWIP_IPV6))
    USEMODULE += ipv6_addr

    USEMODULE += lwip_ipv6
    USEMODULE += lwip_ipv6_autoconfig
  endif
endif

USEMODULE += unicoap
USEMODULE += unicoap_resources_xfa

# This module is needed for CoAP over UDP
USEMODULE += unicoap_driver_udp

# This module is needed for CoAP over DTLS
USEMODULE += unicoap_driver_dtls

# It is okay to import only either of the CoAP over UDP and CoAP over DTLS drivers.

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
