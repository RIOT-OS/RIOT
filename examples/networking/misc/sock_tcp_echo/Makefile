# name of your application
APPLICATION = sock_tcp_echo

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# default to using GNRC
LWIP ?= 0

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

USEMODULE += sock_tcp
USEMODULE += netdev_default

USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += netutils

ifeq (1, $(LWIP))
  USEMODULE += ipv6_addr
  USEMODULE += lwip_ipv6_autoconfig
  USEMODULE += lwip_netdev
else
  USEMODULE += auto_init_gnrc_netif
  USEMODULE += gnrc_ipv6_default
  # we want to be able to open two sockets
  CFLAGS += -DCONFIG_GNRC_TCP_RCV_BUFFERS=2
endif

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# As there is a .config we want to explicitly disable Kconfig by setting
# the variable to empty
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include
