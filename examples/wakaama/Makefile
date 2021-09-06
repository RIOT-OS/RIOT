# name of your application
APPLICATION = wakaama

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..



# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules
USEMODULE += gnrc_ipv6_router_default
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

# NOTE: Add the package for wakaama
USEPKG += wakaama
# Uncomment to enable Wakaama debug log
#CFLAGS += -DCONFIG_LWM2M_WITH_LOGS=1

# Uncomment to indicate that the server is a LwM2M bootstrap server
# CFLAGS += -DCONFIG_LWM2M_BOOTSTRAP=1

include $(RIOTBASE)/Makefile.include

# Configure server via CFLAGS only if not done via Kconfig
ifndef CONFIG_LWM2M_SERVER_URI
  CFLAGS += -DCONFIG_LWM2M_SERVER_URI=$(SERVER_URI)
endif
