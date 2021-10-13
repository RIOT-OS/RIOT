include ../Makefile.tests_common

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules
USEMODULE += gnrc_ipv6_default
USEMODULE += sock_udp

USEMODULE += nanocoap_sock

# Required by test
USEMODULE += od
USEMODULE += shell

include $(RIOTBASE)/Makefile.include
