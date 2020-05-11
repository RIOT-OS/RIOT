DEVELHELP ?= 0
BOARD ?= samr21-xpro
include ../Makefile.tests_common

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
USEMODULE += gnrc_sock_udp
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo

USEMODULE += nanocoap_sock

# include this for printing IP addresses
USEMODULE += shell_commands

# Comment this out to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#DEVELHELP = 1

# Use different settings when compiling for one of the following (low-memory)
# boards
LOW_MEMORY_BOARDS := nucleo-f334r8

# uncomment these to use ethos
#USEMODULE += stdio_ethos gnrc_uhcpc
#
## ethos baudrate can be configured from make command
#ETHOS_BAUDRATE ?= 115200
#CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE)

ifneq (,$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
  $(info Using low-memory configuration for microcoap_server.)
  ## low-memory tuning values
  # lower pktbuf buffer size
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=1000
  USEMODULE += prng_minstd
endif

# include riotboot modules
USEMODULE += riotboot_flashwrite
FEATURES_REQUIRED += riotboot

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
