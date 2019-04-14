include ../Makefile.tests_common

APPLICATION = syslog

CFLAGS += -DRCV_QUEUE_SIZE=8
CFLAGS += -DLSYSLOGD_FEATURE_INPUT_RIOT_IPC
CFLAGS += -DLSYSLOGD_FEATURE_OUTPUT_STDOUT
CFLAGS += -DLSYSLOGD_FEATURE_OUTPUT_REMOTE
CFLAGS += -DLSYSLOGD_RIOT_IPC_INPUT_STACKSIZE=2048
CFLAGS += -DTHREAD_STACKSIZE_MAIN=2048

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
USEMODULE += posix_sockets
USEMODULE += sock_util
USEPKG    += lsyslogd

BOARD_WHITELIST := native

TEST_ON_CI_WHITELIST += all
include $(RIOTBASE)/Makefile.include
