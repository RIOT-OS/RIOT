APPLICATION = netdev_test
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031

DISABLE_MODULE = auto_init

FEATURES_REQUIRED += periph_timer # xtimer required for this application

USEMODULE += gnrc
USEMODULE += gnrc_neterr
USEMODULE += gnrc_netif
USEMODULE += gnrc_netdev
USEMODULE += netdev_test
USEMODULE += od

CFLAGS += -DGNRC_PKTBUF_SIZE=200

include $(RIOTBASE)/Makefile.include
