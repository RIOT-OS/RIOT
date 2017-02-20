# name of your application
APPLICATION = microcoap_server

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := chronos msb-430 msb-430h nucleo-f030 nucleo32-f042 \
                             pca10000 pca10005 stm32f0discovery telosb weio z1 \
                             nucleo32-f031 nucleo-l053

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_udp
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo

#
USEMODULE += gnrc_sock_udp

USEPKG += microcoap
CFLAGS += -DMICROCOAP_DEBUG

# include this for printing IP addresses
USEMODULE += shell_commands

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#CFLAGS += -DDEVELHELP

# Use different settings when compiling for one of the following (low-memory)
# boards
LOW_MEMORY_BOARDS := nucleo-f334

ifneq (,$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
$(info Using low-memory configuration for microcoap_server.)
## low-memory tuning values
# lower pktbuf buffer size
CFLAGS += -DGNRC_PKTBUF_SIZE=1000
USEMODULE += prng_minstd
endif

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

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
