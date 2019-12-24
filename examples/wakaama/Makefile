# name of your application
APPLICATION = wakaama

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..



# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_sock_udp
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Specific the server URI  address (NOTE: Domain names not supported yet)
SERVER_URI ?= '"coap://[fd00:dead:beef::1]"'

ifneq (,$(SERVER_URI))
  CFLAGS += -DLWM2M_SERVER_URI=$(SERVER_URI)
endif

# NOTE: Add the package for wakaama
USEPKG += wakaama
# Uncomment to enable Wakaama debug log
#CFLAGS += -DLWM2M_WITH_LOGS

# Uncomment to indicate that the server is a LwM2M bootstrap server
# CFLAGS += -DLWM2M_BOOTSTRAP=1

# NOTE: Use wakaama in client mode
CFLAGS += -DLWM2M_CLIENT_MODE

include $(RIOTBASE)/Makefile.include
