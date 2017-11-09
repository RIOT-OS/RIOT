# name of your application
APPLICATION = lwmac

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# Currently, LWMAC is only tested and evaluated through on samr21-xpro.
# Once LWMAC has also been tested through on other boards, the whitelist should be
# then accordingly extended.
BOARD_WHITELIST := samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Uncomment these lines if you want to use platform support from external
# repositories:
#RIOTCPU ?= $(CURDIR)/../../RIOT/thirdparty_cpu
#RIOTBOARD ?= $(CURDIR)/../../RIOT/thirdparty_boards

# Uncomment this to enable scheduler statistics for ps:
#CFLAGS += -DSCHEDSTATISTICS

# If you want to use native with valgrind, you should recompile native
# with the target all-valgrind instead of all:
# make -B clean all-valgrind

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
# include and auto-initialize all available sensors
USEMODULE += saul_default

BOARD_PROVIDES_NETIF := airfy-beacon cc2538dk fox iotlab-m3 iotlab-a8-m3 mulle \
        microbit native nrf51dongle nrf52dk nrf6310 openmote-cc2538 pba-d-01-kw2x \
        remote-pa remote-reva saml21-xpro samr21-xpro \
        spark-core telosb yunjia-nrf51822 z1

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
# We use only the lower layers of the GNRC network stack, hence, we can
# reduce the size of the packet buffer a bit
CFLAGS += -DGNRC_PKTBUF_SIZE=512

FEATURES_OPTIONAL += config

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
DEFAULT_CHANNEL ?= 26
CFLAGS += -DIEEE802154_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
