# name of your application
APPLICATION = gnrc_schc

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
USEMODULE += gnrc_schc
USEMODULE += ps

BOARD ?= native
RIOTBASE ?= ../../

# Turn on developer helpers
DEVELHELP ?= 1

USEPKG += libschc

# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=512
endif


include $(RIOTBASE)/Makefile.include
