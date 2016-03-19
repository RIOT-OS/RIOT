APPLICATION = pcap_dev_ieee802154
include ../Makefile.tests_common

BOARD_WHITELIST := native

DISABLE_MODULE += auto_init

USEMODULE += netdev2_ieee802154
USEMODULE += pcap_dev
USEMODULE += pcap_dev_sd_file
USEMODULE += od
USEMODULE += shell
USEMODULE += shell_commands

CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include
