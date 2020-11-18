# name of your application
APPLICATION = gnrc_lorawan

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_lorawan
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_neterr

BOARD ?= b-l072z-lrwan1
RIOTBASE ?= ../../

# Turn on developer helpers
DEVELHELP ?= 1

# use SX1276 by default
DRIVER ?= sx1276

USEMODULE += $(DRIVER)

########################### COMPILE TIME CONFIGURATION ########################
# NOTE: The following options can be configured on runtime as well using
# `ifconfig`

# OTAA compile time configuration keys
CFLAGS += -DCONFIG_LORAMAC_APP_KEY_DEFAULT=\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\"
CFLAGS += -DCONFIG_LORAMAC_APP_EUI_DEFAULT=\"BBBBBBBBBBBBBBBB\"
CFLAGS += -DCONFIG_LORAMAC_DEV_EUI_DEFAULT=\"CCCCCCCCCCCCCCCC\"

# Uncomment and replace with proper keys for joining with ABP
# For TTN, It's necessary to set the RX2 DR to 3
# NOTE: This values will be overriten in case of OTAA.
#CFLAGS += -DLORAMAC_DEV_ADDR_DEFAULT=\"00000000\"
#CFLAGS += -DCONFIG_LORAMAC_NWK_SKEY_DEFAULT=\"00000000000000000000000000000000\"
#CFLAGS += -DCONFIG_LORAMAC_APP_SKEY_DEFAULT=\"00000000000000000000000000000000\"
#CFLAGS += -DLORAMAC_DEFAULT_RX2_DR=LORAMAC_DR_3

# Comment/uncomment as necessary
CFLAGS += -DLORAMAC_DEFAULT_JOIN_PROCEDURE=LORAMAC_JOIN_OTAA
#CFLAGS += -DLORAMAC_DEFAULT_JOIN_PROCEDURE=LORAMAC_JOIN_ABP

# Uncomment to set the highest DR for the EU868 in order to maximize throughput.
# If uncommented, the default value (DR0) is used.
# Note this value is also used for the OTAA.
#CFLAGS += -DLORAMAC_DEFAULT_DR=LORAMAC_DR_5

# Set default messages to unconfirmable
CFLAGS += -DLORAMAC_DEFAULT_TX_MODE=LORAMAC_TX_CNF

include $(RIOTBASE)/Makefile.include

# We can reduce the size of the packet buffer for LoRaWAN, since there's no IP
# support. This will reduce RAM consumption.
# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=512
endif

# Set region
ifeq (,$(CONFIG_LORAMAC_REGION_EU_868)$(CONFIG_LORAMAC_REGION_IN_865))
    CFLAGS += -DCONFIG_LORAMAC_REGION_EU_868=1
endif
