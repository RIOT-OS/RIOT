# name of your application
APPLICATION = benchmark_udp

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# use GNRC by default
LWIP ?= 0

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
ifeq (0,$(LWIP))
  USEMODULE += auto_init_gnrc_netif
  USEMODULE += gnrc_ipv6_default
else
  USEMODULE += lwip_ipv6
endif

# Add also the shell, some shell commands
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6

# Add the benchmark module
USEMODULE += benchmark_udp

# Uncomment this to automatically start sending packets to a pre-defined
# benchmark server
#
# USEMODULE += auto_init_benchmark_udp
# CFLAGS += -DBENCH_SERVER_DEFAULT=\"fd00:dead:beef::1\"
# CFLAGS += -DBENCH_PORT_DEFAULT=12345

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 0

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
