BOARD ?= samr21-xpro

include ../Makefile.pkg_common

USB_VID ?= $(USB_VID_TESTING)
USB_PID ?= $(USB_PID_TESTING)

CLASS ?= tinyusb_class_net_rndis

USEPKG += tinyusb

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += tinyusb_netdev
USEMODULE += $(CLASS)

include $(RIOTBASE)/Makefile.include
