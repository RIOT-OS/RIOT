APPLICATION = driver_cc112x
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi periph_gpio

USEMODULE += cc112x
USEMODULE += auto_init_gnrc_netif
USEMODULE += auto_init
USEMODULE += gnrc_netif
USEMODULE += gnrc_nomac
USEMODULE += gnrc_pktdump
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += xtimer

CFLAGS += -DDEVELHELP -Wall

include $(RIOTBASE)/Makefile.include
