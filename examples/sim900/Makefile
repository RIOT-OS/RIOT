# name of your application
APPLICATION = sim900

FEATURES_REQUIRED = periph_uart
FEATURES_REQUIRED += periph_timer

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

GNRC_NETIF_NUMOF := 2

USEMODULE = ipv4_addr
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += xtimer
USEMODULE += gnrc_ppp
USEMODULE += sim900
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_sixlowpan_border_router_default
USEMODULE += fib


include $(RIOTBASE)/Makefile.include
