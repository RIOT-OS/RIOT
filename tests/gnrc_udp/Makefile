include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-leonardo \
                             arduino-mega2560 arduino-nano \
                             arduino-uno calliope-mini chronos hifive1 hifive1b \
                             i-nucleo-lrwan1 \
                             mega-xplained microbit msb-430 msb-430h \
                             nucleo-f031k6 nucleo-f042k6 nucleo-f303k8 \
                             nucleo-l031k6 nucleo-f030r8 nucleo-f070rb \
                             nucleo-f072rb nucleo-f103rb nucleo-f302r8 \
                             nucleo-f334r8 nucleo-l053r8 spark-core \
                             stm32f0discovery stm32l0538-disco telosb \
                             waspmote-pro wsn430-v1_3b \
                             wsn430-v1_4 z1

USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_pktbuf_cmd
USEMODULE += gnrc_udp
USEMODULE += gnrc_rpl
USEMODULE += auto_init_gnrc_rpl
USEMODULE += od
USEMODULE += gnrc_icmpv6_echo
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTMAKE)/default-channel.inc.mk
