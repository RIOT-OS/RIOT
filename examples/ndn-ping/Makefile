# name of your application
APPLICATION = ndn_ping

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../

BOARD_INSUFFICIENT_MEMORY := chronos msb-430 msb-430h telosb weio wsn430-v1_3b wsn430-v1_4 z1 \
                             nucleo-f042k6 nucleo-f031k6 nucleo-l031k6 nucleo-f030r8 nucleo-l053r8 \
                             stm32f0discovery

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
