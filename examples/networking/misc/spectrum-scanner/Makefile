# Set the name of your application:
APPLICATION = spectrum-scanner

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Needed for print_float()
FEATURES_REQUIRED += float_math

# Define modules that are used
USEMODULE += gnrc
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += xtimer
USEMODULE += ztimer64_xtimer_compat
USEMODULE += fmt

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
