APPLICATION = sock_secyure_echo

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Minimum setting
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default

# NOTE: Network stack must be defined by the user not by TLSMAN
USEMODULE += gnrc_sock_udp

USEMODULE += sock_secure
USEPKG += tinydtls

# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps


# Special include directory with the (D)TLS keys required for the test
INCLUDES += -I$(APPDIR)/keys

# Default DTLS Server for the test
REM_SERVER ?='"fd00:dead:beef::1"'
CFLAGS += -DREMOTE_SERVER=$(REM_SERVER)

# FIXME: This is a temporary patch
# TinyDTLS <= 0.8.6 requires around 426 bytes in RAM.
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(3*THREAD_STACKSIZE_DEFAULT\)

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
