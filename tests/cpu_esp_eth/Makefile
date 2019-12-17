BOARD ?=esp32-olimex-evb

include ../Makefile.tests_common

FEATURES_REQUIRED += arch_esp32
FEATURES_REQUIRED += esp_eth

USEMODULE += auto_init_gnrc_netif
USEMODULE += esp_eth
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

include $(RIOTBASE)/Makefile.include
