BOARD ?= nrf52dk
include ../Makefile.drivers_common

# include nrfmin, the main purpose of this test
USEMODULE += nrfmin
# use a minimal GNRC configuration
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6
USEMODULE += gnrc_icmpv6_echo
# also add the shell with some basic shell commands
USEMODULE += shell_cmds_default

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
