# name of your application
APPLICATION = rdcli

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-mega2560 arduino-uno \
                             chronos hifive1 mega-xplained msb-430 msb-430h  \
                             nucleo-f030r8 nucleo-l053r8 nucleo-f031k6 \
                             nucleo-f042k6 nucleo-f303k8 nucleo-f334r8 \
                             nucleo-l031k6 stm32f0discovery telosb waspmote-pro \
                             wsn430-v1_3b wsn430-v1_4 z1

USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo

USEMODULE += cord_ep_standalone

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += fmt

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# For debugging and demonstration purposes, we limit the lifetime to 60s
CFLAGS += -DRDCLI_LT=60

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
