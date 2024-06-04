BOARD ?= samr21-xpro

include ../Makefile.pkg_common

USEPKG += openwsn
USEMODULE += openwsn_openstack
USEMODULE += openwsn_scheduler
USEMODULE += sock_udp

# Mock OpenWSN scheduler to handle the udp_transmit task.
USEMODULE += openwsn_scheduler_mock
EXTERNAL_MODULE_DIRS += external_modules

# Explicitly include ztimer to enable timeout
USEMODULE += ztimer_usec
DISABLE_MODULE += auto_init_openwsn

CFLAGS +=-DCONFIG_OPENWSN_NETIF_ID=31

include $(RIOTBASE)/Makefile.include
