# name of your application
APPLICATION = ndn_ping

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../

# Do not build for any board that supports nimble_netif, as NimBLE and ndn-riot
# use different crypto libraries that have name clashes (tinycrypt vs uECC)
BOARD_BLACKLIST := acd52832 nrf52832-mdk nrf52dk ruuvitag thingy52

# Include packages that pull up and auto-init the link layer.
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += random
USEMODULE += shell
USEMODULE += shell_commands

USEPKG += ndn-riot

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
