# name of your application
APPLICATION = suit_update

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-mega2560 arduino-nano \
                             arduino-uno b-l072z-lrwan1 chronos lsn50 msb-430 \
                             msb-430h nucleo-f031k6 nucleo-f042k6 nucleo-l031k6 \
                             nucleo-f030r8 nucleo-f302r8 nucleo-f303k8 \
                             nucleo-f334r8 nucleo-l053r8 nucleo-l073rz ruuvitag \
                             saml10-xpro saml11-xpro stm32f0discovery thingy52 \
                             telosb waspmote-pro wsn430-v1_3b wsn430-v1_4 z1

# lower pktbuf size to something sufficient for this application
CFLAGS += -DGNRC_PKTBUF_SIZE=2000

#
# Networking
#
# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present

# uncomment this to compile in support for a possibly available radio
#USEMODULE += gnrc_netdev_default

USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
USEMODULE += gnrc_sock_udp
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo

# include this for printing IP addresses
USEMODULE += shell_commands

# Set this to 1 to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 0

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

#
# SUIT update specific stuff
#

USEMODULE += nanocoap_sock sock_util
USEMODULE += suit suit_coap

# SUIT draft v4 support:
USEMODULE += suit_v4

# Change this to 0 to not use ethos
USE_ETHOS ?= 1

ifeq (1,$(USE_ETHOS))
  GNRC_NETIF_NUMOF := 2
  USEMODULE += stdio_ethos
  USEMODULE += gnrc_uhcpc

  # ethos baudrate can be configured from make command
  ETHOS_BAUDRATE ?= 115200
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE)

  # make sure ethos and uhcpd are built
  TERMDEPS += host-tools

  # For local testing, run
  #
  #     $ cd dist/tools/ethos; sudo ./setup_network.sh riot0 2001:db8::0/64
  #
  #... in another shell and keep it running.
  export TAP ?= riot0
  TERMPROG = $(RIOTTOOLS)/ethos/ethos
  TERMFLAGS = $(TAP) $(PORT)
endif

# The test needs the linked slot binaries without header in order to be able to
# create final binaries with specific APP_VER values. The CI RasPi test workers
# don't compile themselves and re-create signed images, thus add the required
# files here so they will be submitted along with the test jobs.
TEST_EXTRA_FILES += $(SLOT_RIOT_ELFS) $(SUIT_SEC) $(SUIT_PUB)

# Due to issues with source address selection, this test currently might not
# run reliably on CI.
# See #12404, #12408.
TEST_ON_CI_BLACKLIST += all

include $(RIOTBASE)/Makefile.include

.PHONY: host-tools

host-tools:
	$(Q)env -u CC -u CFLAGS make -C $(RIOTTOOLS)

include $(RIOTMAKE)/default-channel.inc.mk
