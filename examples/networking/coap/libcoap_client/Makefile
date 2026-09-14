# name of your application
APPLICATION = libcoap_client

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif

# Activate ICMPv6 error messages
USEMODULE += gnrc_icmpv6_error

# Specify the mandatory networking module for a IPv6 routing node
USEMODULE += gnrc_ipv6_router_default

# Add a routing protocol
USEMODULE += gnrc_rpl
USEMODULE += auto_init_gnrc_rpl

# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo

# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_default
USEMODULE += memarray
USEMODULE += ipv4_addr

# a cryptographically secure implementation of PRNG is needed for tinydtls
# Uncomment the following 3 lines for tinydtls support
CFLAGS += -DWITH_RIOT_SOCK
USEPKG += tinydtls
USEMODULE += prng_sha1prng

# libcoap support
USEPKG += libcoap
# Uncomment to enable libcoap OSCORE support
# USEMODULE += libcoap_oscore

# Configure if DNS is required
# USEMODULE += sock_dns

# Support 64 bit ticks
USEMODULE += ztimer64_xtimer_compat

# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6
USEMODULE += netstats_rpl

# libcoap needs some space
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(3*THREAD_STACKSIZE_DEFAULT\)

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
# QUIET ?= 1

include $(RIOTBASE)/Makefile.include
