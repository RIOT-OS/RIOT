include ../Makefile.tests_common

USEPKG += zboss

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif

include $(RIOTBASE)/Makefile.include
