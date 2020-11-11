# use samr21-xpro as default:
BOARD ?= samr21-xpro
include ../Makefile.tests_common

# Currently, LWMAC is only tested and evaluated through on samr21-xpro.
# Once LWMAC has also been tested through on other boards, the whitelist should
# be then accordingly extended.
BOARD_WHITELIST := samr21-xpro

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
# Use modules for networking
# gnrc is a meta module including all required, basic gnrc networking modules
USEMODULE += gnrc
# use the default network interface for the board
USEMODULE += gnrc_netdev_default
# automatically initialize the network interface
USEMODULE += auto_init_gnrc_netif
# shell command to send L2 packets with a simple string
USEMODULE += gnrc_txtsnd
# the application dumps received packets to stdout
USEMODULE += gnrc_pktdump
# Use LWMAC
USEMODULE += gnrc_lwmac

include $(RIOTBASE)/Makefile.include

# We use only the lower layers of the GNRC network stack, hence, we can
# reduce the size of the packet buffer a bit
# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=512
endif

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
