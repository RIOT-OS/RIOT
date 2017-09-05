APPLICATION = stm32_eth_lwip
include ../Makefile.tests_common

BOARD := stm32f4discovery

FEATURES_REQUIRED += periph_stm32_eth

USEMODULE += lwip
USEMODULE += lwip_netdev
USEMODULE += lwip_udp
USEMODULE += lwip_sock_udp
USEMODULE += lwip_ethernet
USEMODULE += lwip_ipv4
USEMODULE += lwip_arp
USEMODULE += netdev_eth

include $(RIOTBASE)/Makefile.include
