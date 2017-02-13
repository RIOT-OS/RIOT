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
        pca10000 pca10005 remote-pa remote-reva saml21-xpro samr21-xpro \
        spark-core telosb yunjia-nrf51822 z1

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

FEATURES_OPTIONAL += config
FEATURES_OPTIONAL += periph_rtc

ifneq (,$(filter msb-430,$(BOARD)))
  USEMODULE += sht11
endif
ifneq (,$(filter msba2,$(BOARD)))
  USEMODULE += sht11
  USEMODULE += mci
  USEMODULE += random
endif

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
ifneq (,$(filter cc110x,$(USEMODULE)))          # radio is cc110x sub-GHz
  DEFAULT_CHANNEL ?= 0
  CFLAGS += -DCC110X_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
else
  ifneq (,$(filter at86rf212b,$(USEMODULE)))    # radio is IEEE 802.15.4 sub-GHz
    DEFAULT_CHANNEL ?= 5
    CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(DEFAULT_CHANNEL)
  else                                          # radio is IEEE 802.15.4 2.4 GHz
    DEFAULT_CHANNEL ?= 26
    CFLAGS += -DIEEE802154_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
  endif
endif
