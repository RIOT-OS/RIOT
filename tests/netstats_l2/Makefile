APPLICATION = netstats
include ../Makefile.tests_common

BOARD_PROVIDES_NETIF := airfy-beacon fox iotlab-m3 mulle native nrf51dongle \
	nrf6310 pba-d-01-kw2x pca10000 pca10005 saml21-xpro samr21-xpro spark-core \
	yunjia-nrf51822

BOARDS ?= $(shell find $(RIOTBASE)/boards/* -maxdepth 0 -type d \! -name "*-common" -exec basename {} \;)

BOARD_BLACKLIST := $(filter-out $(BOARD_PROVIDES_NETIF), $(BOARDS))

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += gnrc
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += netstats_l2

include $(RIOTBASE)/Makefile.include
