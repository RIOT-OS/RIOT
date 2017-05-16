APPLICATION = driver_xbee
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_uart periph_gpio

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031 nucleo32-f042 nucleo-f030 nucleo-f334 \
                             stm32f0discovery weio

USEMODULE += xbee
USEMODULE += gnrc_netif
USEMODULE += gnrc_txtsnd
USEMODULE += gnrc_netdev
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_pktdump
USEMODULE += shell
USEMODULE += shell_commands

# No need of big buffer for this test
CFLAGS += -DGNRC_PKTBUF_SIZE=512

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# add current directory to the include path. Putting it in CFLAGS will make
# it go to the beginning, before the standard includes.
# That way xbee_params.h get's included and auto configuration can pick it up.
CFLAGS += -I$(CURDIR)

include $(RIOTBASE)/Makefile.include
