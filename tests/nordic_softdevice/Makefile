BOARD ?= nrf52dk
include ../Makefile.tests_common

# Use the Nordic SoftDevice
USEPKG += nordic_softdevice_ble

# use a minimal GNRC configuration
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6
USEMODULE += gnrc_icmpv6_echo
# also add the shell with some basic shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

TEST_ON_CI_WHITELIST += nrf52dk

include $(RIOTBASE)/Makefile.include
