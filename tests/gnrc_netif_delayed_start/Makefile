include ../Makefile.tests_common

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# gnrc is a meta module including all required, basic gnrc networking modules
USEMODULE += gnrc

# manually initialize the network interface
USEMODULE += gnrc_netif_init_devs

# pull in the GNRC IP stack to see how everything plays together
USEMODULE += gnrc_ipv6_default

# enable ICMP ping
USEMODULE += gnrc_icmpv6_echo

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
