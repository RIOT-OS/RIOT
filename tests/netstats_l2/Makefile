include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := i-nucleo-lrwan1

# This application runs by default on boards providing the netif feature
FEATURES_REQUIRED += netif

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += gnrc
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += netstats_l2

# Cannot run the test on `murdock` in `native`
#   open(/dev/net/tun): No such file or directory
TEST_ON_CI_BLACKLIST += native

include $(RIOTBASE)/Makefile.include
