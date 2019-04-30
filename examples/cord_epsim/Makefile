# name of your application
APPLICATION = cord_epsim

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-mega2560 arduino-nano \
                             arduino-uno chronos hifive1 msb-430 msb-430h \
                             nucleo-f030r8 nucleo-l053r8 nucleo-f031k6 \
                             nucleo-f042k6 nucleo-f303k8 nucleo-f334r8 \
                             nucleo-l031k6 mega-xplained stm32f0discovery \
                             telosb waspmote-pro wsn430-v1_3b wsn430-v1_4 z1

# Enable GNRC networking
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default

# Run the simple CoRE resource directory
USEMODULE += cord_epsim

# We also use the xtimer for periodic RD entry updates in this example
USEMODULE += xtimer

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# For debugging and demonstration purposes, we limit the lifetime to the minimal
# allowed value of 60s (see draft-ietf-core-resource-directory-11, Table 2)
RD_LT ?= 60
CFLAGS += -DCORD_LT=$(RD_LT)

# If nothing else is defined, we use CoAP default port
RD_PORT ?= COAP_PORT
CFLAGS += -DRD_PORT=$(RD_PORT)

# The RD server's address must be defined by the build environment by setting
# the RD_ADDR environment variable. Per default, this value is set to the
# loopback address for enabling the build tests to successfully build this
# example.
RD_ADDR ?= \"affe::1\"
CFLAGS += -DRD_ADDR=$(RD_ADDR)

include $(RIOTBASE)/Makefile.include
