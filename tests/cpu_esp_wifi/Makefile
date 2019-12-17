BOARD ?= esp32-wroom-32

include ../Makefile.tests_common

FEATURES_REQUIRED += arch_esp
FEATURES_REQUIRED += esp_wifi

USEMODULE += auto_init_gnrc_netif
USEMODULE += esp_wifi
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

include $(RIOTBASE)/Makefile.include
