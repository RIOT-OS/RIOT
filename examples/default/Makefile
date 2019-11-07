# name of your application
APPLICATION = default

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Uncomment these lines if you want to use platform support from external
# repositories:
#RIOTCPU ?= $(CURDIR)/../../RIOT/thirdparty_cpu
#RIOTBOARD ?= $(CURDIR)/../../RIOT/thirdparty_boards

# Uncomment this to enable scheduler statistics for ps:
#USEMODULE += schedstatistics

# If you want to use native with valgrind, you should recompile native
# with the target all-valgrind instead of all:
# make -B clean all-valgrind

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
# include and auto-initialize all available sensors
USEMODULE += saul_default

BOARD_PROVIDES_NETIF := acd52832 airfy-beacon atmega256rfr2-xpro avr-rss2 b-l072z-lrwan1 cc2538dk fox \
        hamilton iotlab-m3 iotlab-a8-m3 lobaro-lorabox lsn50 mulle microbit msba2 \
        microduino-corerf native nrf51dk nrf51dongle nrf52dk nrf52840dk nrf52840-mdk nrf6310 \
        nucleo-f767zi openmote-b openmote-cc2538 pba-d-01-kw2x remote-pa remote-reva \
        ruuvitag samr21-xpro samr30-xpro spark-core telosb thingy52 yunjia-nrf51822 z1

ifneq (,$(filter $(BOARD),$(BOARD_PROVIDES_NETIF)))
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

  # We use only the lower layers of the GNRC network stack, hence, we can
  # reduce the size of the packet buffer a bit
  CFLAGS += -DGNRC_PKTBUF_SIZE=512
endif

FEATURES_OPTIONAL += periph_rtc

ifneq (,$(filter msba2,$(BOARD)))
  USEMODULE += mci
  USEMODULE += random
endif

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
