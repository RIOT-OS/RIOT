# name of your application
APPLICATION = cord_lc

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo

USEMODULE += cord_lc

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# The default size of GCOAP_PDU_BUF_SIZE can be too small for the response
# from the RD server. Use a larger value if the client drops response packet
# from RD server.
CFLAGS += -DCONFIG_GCOAP_PDU_BUF_SIZE=512

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
