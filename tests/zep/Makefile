APPLICATION = zep
include ../Makefile.tests_common

# though it would work in theory on other boards I'm currently not willing/to
# time-constraint to put effort into this for other boards.
BOARD_WHITELIST = native

USEMODULE += gnrc_zep
USEMODULE += gnrc_netif_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_nomac
USEMODULE += gnrc_pktdump
USEMODULE += shell
USEMODULE += shell_commands


# set optional default values for ZEP parameters if unset
ZEP_DST      ?= "\"::1\""
ZEP_SRC_PORT ?= GNRC_ZEP_DEFAULT_PORT
ZEP_DST_PORT ?= GNRC_ZEP_DEFAULT_PORT

# export ZEP parameters
CFLAGS += -DZEP_DST="\"$(ZEP_DST)\""
CFLAGS += -DZEP_SRC_PORT=$(ZEP_SRC_PORT)
CFLAGS += -DZEP_DST_PORT=$(ZEP_DST_PORT)

# one for gnrc_netif_default and one for ZEP
CFLAGS += -DGNRC_NETIF_NUMOF=2

include $(RIOTBASE)/Makefile.include
