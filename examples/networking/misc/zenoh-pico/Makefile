APPLICATION = zenoh-pico

# If no BOARD is found in the environment, use this default:
BOARD ?= nrf52840dk

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

USEPKG += zenoh-pico
# also add peer mode to the build, two nodes communicate without a zenoh
# router: it must be set for the package and the application
CFLAGS += -DZ_FEATURE_UNICAST_PEER=1

# IPv6 over the network interfaces of the board (6LoWPAN over IEEE 802.15.4,
# Ethernet...), TCP for zenoh
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_sock_tcp
USEMODULE += gnrc_tcp

# native: the tap interface of netdev_default, or a simulated IEEE 802.15.4
# radio with USE_ZEP=1 (dist/tools/zep_dispatch)
USE_ZEP ?= 0
ifeq (1,$(USE_ZEP))
  USEMODULE += netdev
  USEMODULE += socket_zep
else
  USEMODULE += netdev_default
endif

USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps

# the side that closes a TCP connection first waits 2 x CONFIG_GNRC_TCP_MSL_MS
# in TIME_WAIT, 2 x 30 s by default: 2 x 1 s
CFLAGS += -DCONFIG_GNRC_TCP_MSL_MS=1000

# z_open() runs in the shell, on the stack of the main thread: 3 KiB more than
# the default (1.5 KiB on Cortex-M)
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(THREAD_STACKSIZE_DEFAULT+3072\)

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include

# Set a custom channel and PAN ID if needed (DEFAULT_CHANNEL, DEFAULT_PAN_ID)
include $(RIOTMAKE)/default-radio-settings.inc.mk
