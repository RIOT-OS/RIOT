# name of your application
APPLICATION = suit_update

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

#
# Networking
#
# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present

# uncomment this to compile in support for a possibly available radio
#USEMODULE += netdev_default

USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_router_default
USEMODULE += sock_udp
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo

# include this for printing IP addresses
USEMODULE += shell
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

USEMODULE += suit suit_transport_coap

# Display a progress bar during firmware download
USEMODULE += progress_bar

# Optional feature to trigger suit update through gpio callback
FEATURES_OPTIONAL += periph_gpio_irq

# Default COAP manifest resource location when fetched through gpio trigger
CFLAGS += -DSUIT_MANIFEST_RESOURCE=\"$(SUIT_COAP_ROOT)/$(SUIT_NOTIFY_MANIFEST)\"

# Enable test_utils_interactive_sync, only used when running automatic test
DEFAULT_MODULE += test_utils_interactive_sync

ifeq ($(BOARD),native)
  USE_ETHOS ?= 0
  # Configure two RAM regions with 2K each
  CFLAGS += -DCONFIG_SUIT_STORAGE_RAM_REGIONS=2 -DCONFIG_SUIT_STORAGE_RAM_SIZE=2048
endif

# Change this to 0 to not use ethos
USE_ETHOS ?= 1

ifeq (1,$(USE_ETHOS))
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

# Ensure both slot bin files are always generated and linked to avoid compiling
# during the test. This ensures that "BUILD_IN_DOCKER=1 make test"
# can rely on them being present without having to trigger re-compilation.
BUILD_FILES += $(SLOT_RIOT_ELFS:%.elf=%.bin)

# The test needs the linked slot binaries without header in order to be able to
# create final binaries with specific APP_VER values. The CI RasPi test workers
# don't compile themselves and re-create signed images, thus add the required
# files here so they will be submitted along with the test jobs.
TEST_EXTRA_FILES += $(SLOT_RIOT_ELFS) $(SUIT_SEC) $(SUIT_PUB)
# For now `examples/%/tests" still rely on the test applicaton being reset after
# a terminal is opened to synchronize.
TESTRUNNER_RESET_AFTER_TERM ?= 1

# This can be removed as soon as the Pi-fleet runners support an Openssl version
# with ed25519 support.
TEST_ON_CI_BLACKLIST = all

include $(RIOTBASE)/Makefile.include

# allow to use large blocks to utilize large MTUs (802.15.4g, Ethernet, WiFi)
LARGE_BLOCKS ?= 0
ifeq (1, $(LARGE_BLOCKS))
  CFLAGS += -DCONFIG_SUIT_COAP_BLOCKSIZE=COAP_BLOCKSIZE_1024
else
# lower pktbuf size to something sufficient for this application
# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=2000
endif
endif

.PHONY: host-tools

host-tools:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)

include $(RIOTMAKE)/default-radio-settings.inc.mk
