include ../Makefile.net_common

FEATURES_REQUIRED += netif

USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += gnrc
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += netstats_l2

# Cannot run the test on `murdock` in `native`
#   open(/dev/net/tun): No such file or directory
TEST_ON_CI_BLACKLIST += native32 native64

# sometimes fails on esp32, see #14237.
TEST_ON_CI_BLACKLIST += esp32-wroom-32

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
