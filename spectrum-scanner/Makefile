# Set the name of your application:
APPLICATION = spectrum-scanner

# If no BOARD is found in the environment, use this default:
BOARD ?= frdm-kw41z

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../RIOT

# Define modules that are used
USEMODULE += gnrc
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += xtimer
USEMODULE += fmt

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
