BOARD ?= nrf52dk
include ../Makefile.tests_common

# include nrfmin, the main purpose of this test
USEMODULE += nrfmin
# use a minimal GNRC configuration
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6
USEMODULE += gnrc_icmpv6_echo
# also add the shell with some basic shell commands
USEMODULE += shell
USEMODULE += shell_commands

include $(RIOTBASE)/Makefile.include
