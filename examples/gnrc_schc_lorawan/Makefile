# name of your application
APPLICATION = gnrc_schc_lorawan

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_lorawan
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
USEMODULE += gnrc_schc

BOARD ?= b-l072z-lrwan1
RIOTBASE ?= ../../

# Turn on developer helpers
DEVELHELP ?= 1

# use SX1276 by default
DRIVER ?= sx1276

USEMODULE += $(DRIVER)

USEPKG += libschc

# We can reduce the size of the packet buffer for LoRaWAN, since there's no IP
# support. This will reduce RAM consumption.
# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=512
endif

########################### COMPILE TIME CONFIGURATION ########################
# NOTE: The following options can be configured on runtime as well using
# `ifconfig`

# OTAA compile time configuration keys
CFLAGS += -DLORAMAC_APP_KEY_DEFAULT=\{0x00\,0x00\,0x00\,0x00\,0x00\,0x00\,0x00\,0x00\,0x00\,0x00\,0x00\,0x00\,0x00\,0x00\,0x00\,0x00\}
CFLAGS += -DLORAMAC_APP_EUI_DEFAULT=\{0x00\,0x00\,0x00\,0x00\,0x00\,0x00\,0x00\,0x00\}
CFLAGS += -DLORAMAC_DEV_EUI_DEFAULT=\{0x00\,0x00\,0x00\,0x00\,0x00\,0x00\,0x00\,0x00\}

# Uncomment and replace with proper keys for joining with ABP
# For TTN, It's necessary to set the RX2 DR to 3
# NOTE: This values will be overriten in case of OTAA.
# CFLAGS += -DLORAMAC_DEV_ADDR_DEFAULT=\{0x00\,0x1f\,0x7e\,0x3a\}
# CFLAGS += -DLORAMAC_NWK_SKEY_DEFAULT=\{0x79\,0x99\,0x94\,0xc9\,0xb2\,0x8a\,0xe7\,0xc7\,0xc9\,0x2b\,0x44\,0xf1\,0xe3\,0x83\,0xd1\,0xa7\}
# CFLAGS += -DLORAMAC_APP_SKEY_DEFAULT=\{0xb6\,0xde\,0xa1\,0xcd\,0x69\,0x82\,0xa2\,0x98\,0xc6\,0xe2\,0x54\,0x15\,0x9f\,0xf3\,0xab\,0xcd\}
# CFLAGS += -DLORAMAC_DEFAULT_RX2_DR=LORAMAC_DR_3

# Comment/uncomment as necessary
CFLAGS += -DLORAMAC_DEFAULT_JOIN_PROCEDURE=LORAMAC_JOIN_OTAA
#CFLAGS += -DLORAMAC_DEFAULT_JOIN_PROCEDURE=LORAMAC_JOIN_ABP

CFLAGS += -DLORAMAC_DEFAULT_TX_MODE=LORAMAC_TX_UNCNF
CFLAGS += -DLORAMAC_DEFAULT_DR=LORAMAC_DR_5
CFLAGS += -DLORAMAC_DEFAULT_PUBLIC_NETWORK=false

# Uncomment to set the highest DR for the EU868 in order to maximize throughput.
# If uncommented, the default value (DR0) is used.
# Note this value is also used for the OTAA.
#CFLAGS += -DLORAMAC_DEFAULT_DR=LORAMAC_DR_5

# Uncomment the following 2 lines to specify static link lokal IPv6 address
# this might be useful for testing, in cases where you cannot or do not want to
# run a shell with ifconfig to get the real link lokal address.
IPV6_STATIC_LLADDR ?= '"fe80::8C0D:14FF:FE1f:E1D8"'
CFLAGS += -DGNRC_IPV6_STATIC_LLADDR=$(IPV6_STATIC_LLADDR)

include $(RIOTBASE)/Makefile.include
