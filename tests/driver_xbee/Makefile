include ../Makefile.tests_common

USEMODULE += xbee
USEMODULE += gnrc_txtsnd
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_pktdump
USEMODULE += shell
USEMODULE += shell_commands

# No need of big buffer for this test
CFLAGS += -DGNRC_PKTBUF_SIZE=512

# add current directory to the include path. Putting it in CFLAGS will make
# it go to the beginning, before the standard includes.
# That way xbee_params.h get's included and auto configuration can pick it up.
CFLAGS += -I$(CURDIR)

include $(RIOTBASE)/Makefile.include
