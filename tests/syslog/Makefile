include ../Makefile.tests_common

APPLICATION = syslog

CFLAGS += -DRCV_QUEUE_SIZE=8
CFLAGS += -DLSYSLOGD_FEATURE_INPUT_RIOT_IPC
CFLAGS += -DLSYSLOGD_FEATURE_OUTPUT_STDOUT
CFLAGS += -DLSYSLOGD_FEATURE_OUTPUT_REMOTE

USEMODULE += syslog
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_udp
USEMODULE += gnrc_sock_udp
USEPKG    += lsyslogd

BOARD_WHITELIST := native

TEST_ON_CI_WHITELIST += all
include $(RIOTBASE)/Makefile.include
