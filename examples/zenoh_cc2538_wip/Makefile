# name of your application
APPLICATION = zeno_publisher

# If no BOARD is found in the environment, use this default:
BOARD ?= cc2538dk

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

USEMODULE += pthread
USEMODULE += posix_inet
USEMODULE += posix_sockets
USEMODULE += gnrc_sock_udp
USEMODULE += gnrc_ipv6_default
USEMODULE += posix_time



DEVELHELP ?= 1
USEPKG += zenoh_cc2538_wip


include $(RIOTBASE)/Makefile.include
