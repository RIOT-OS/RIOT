# name of your application
APPLICATION = rdcli_simple

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := chronos hifive1 msb-430 msb-430h nucleo-f030r8 nucleo-l053r8 \
                             nucleo-f031k6 nucleo-f042k6 nucleo-l031k6 \
                             stm32f0discovery telosb wsn430-v1_3b wsn430-v1_4 \
                             z1 nucleo-f303k8 nucleo-f334r8

# Enable GNRC networking
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default

# Run the simple CoRE resource directory
USEMODULE += rdcli_simple_standalone

# Include the shell for testing purposes
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# For debugging and demonstration purposes, we limit the lifetime to the minimal
# allowed value of 60s (see draft-ietf-core-resource-directory-11, Table 2)
RD_LT ?= 60
# Override this variable to set the RD server address (default is the all nodes
# multicast address)
RD_ADDR ?= \"ff02::1\"
RD_PORT ?= 5683

CFLAGS += -DRDCLI_LT=$(RD_LT)
CFLAGS += -DRDCLI_SERVER_ADDR=$(RD_ADDR)
CFLAGS += -DRDCLI_SERVER_PORT=$(RD_PORT)

include $(RIOTBASE)/Makefile.include
