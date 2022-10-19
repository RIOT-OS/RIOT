# Set the name of your application:
APPLICATION = sniffer

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Define modules that are used
USEMODULE += fmt
USEMODULE += gnrc
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += ztimer64_usec

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
