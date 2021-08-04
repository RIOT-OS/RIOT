include ../Makefile.tests_common
include $(RIOTBASE)/tests/Makefile.boards.netif

BOARD_WHITELIST += $(BOARD_PROVIDES_NETIF)

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += gnrc
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += netstats_l2

# Cannot run the test on `murdock` in `native`
#   open(/dev/net/tun): No such file or directory
TEST_ON_CI_BLACKLIST += native

# sometimes fails on esp32, see #14237.
TEST_ON_CI_BLACKLIST += esp32-wroom-32

include $(RIOTBASE)/Makefile.include
