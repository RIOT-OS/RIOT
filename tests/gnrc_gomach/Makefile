# use samr21-xpro as default:
BOARD ?= samr21-xpro
include ../Makefile.tests_common

# Currently, GoMacH has only been tested and evaluated through on samr21-xpro and iotlab-m3
# nodes. Once GoMacH has also been tested through on other devices, the whitelist should
# be then accordingly extended.
BOARD_WHITELIST := samr21-xpro iotlab-m3

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
# include and auto-initialize all available sensors
USEMODULE += saul_default

# Use modules for networking
# gnrc is a meta module including all required, basic gnrc networking modules
USEMODULE += gnrc
# use the default network interface for the board
USEMODULE += netdev_default
# automatically initialize the network interface
USEMODULE += auto_init_gnrc_netif
# shell command to send L2 packets with a simple string
USEMODULE += gnrc_txtsnd
# the application dumps received packets to stdout
USEMODULE += gnrc_pktdump
# Use GoMacH as the MAC protocol
USEMODULE += gnrc_gomach

include $(RIOTBASE)/Makefile.include

# We use only the lower layers of the GNRC network stack, hence, we can
# reduce the size of the packet buffer a bit
# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=1024
endif

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
