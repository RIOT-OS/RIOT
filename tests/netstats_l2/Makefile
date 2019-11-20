include ../Makefile.tests_common

BOARD_PROVIDES_NETIF ?= airfy-beacon fox iotlab-m3 mulle native nrf51dk nrf51dongle \
	nrf6310 pba-d-01-kw2x samd21-xpro saml21-xpro samr21-xpro spark-core \
	yunjia-nrf51822 msba2

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

include $(RIOTBASE)/Makefile.include
