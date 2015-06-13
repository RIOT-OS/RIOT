APPLICATION = zep
include ../Makefile.tests_common

# though it would work in theory on other boards I'm currently not willing/to
# time-constraint to put effort into this for other boards.
BOARD_WHITELIST = native

USEMODULE += ng_zep
USEMODULE += ng_netif_default
USEMODULE += auto_init_ng_netif
USEMODULE += ng_ipv6_default
USEMODULE += ng_nomac
USEMODULE += ng_pktdump
USEMODULE += shell
USEMODULE += shell_commands

ifeq (,$(ZEP_DST))
  # set default
  CFLAGS += -DZEP_DST="\"::1\""
else
  CFLAGS += -DZEP_DST="\"$(ZEP_DST)\""
endif

ifeq (,$(ZEP_SRC_PORT))
  # set default
  CFLAGS += -DZEP_SRC_PORT=NG_ZEP_DEFAULT_PORT
else
  CFLAGS += -DZEP_SRC_PORT=$(ZEP_SRC_PORT)
endif

ifeq (,$(ZEP_DST_PORT))
  # set default
  CFLAGS += -DZEP_DST_PORT=NG_ZEP_DEFAULT_PORT
else
  CFLAGS += -DZEP_DST_PORT=$(ZEP_DST_PORT)
endif

# add current directory to the include path. Putting it in CFLAGS will make
# it go to the beginning, before the standard includes.
# That way xbee_params.h get's included and auto configuration can pick it up.
CFLAGS += -I$(CURDIR)

# one for ng_netif_default and one for ZEP
CFLAGS += -DNG_NETIF_NUMOF=2

include $(RIOTBASE)/Makefile.include
