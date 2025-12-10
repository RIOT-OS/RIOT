# name of your application
APPLICATION = gnrc_lorawan

# Turn on developer helpers
DEVELHELP ?= 1

BOARD ?= b-l072z-lrwan1
RIOTBASE ?= $(CURDIR)/../../../..

# Include board's default network devices and auto-initialization of GNRC
# interfaces
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif

# Add support for GNRC LoRaWAN
USEMODULE += gnrc_lorawan
# Add support for GNRC LoRaWAN 1.1
# USEMODULE += gnrc_lorawan_1_1

# Use GNRC pktdump to print downlink messages
USEMODULE += gnrc_pktdump

# Include the shell and shell commands.
USEMODULE += shell
USEMODULE += shell_cmds_default

# Use GNRC Txtsnd to transmit LoRaWAN from the shell
USEMODULE += gnrc_txtsnd

# Uncomment as needed if a board doesn't include a LoRa radio by default
# USEMODULE += sx1272
# USEMODULE += sx1276

# As there is an 'app.config' we want to explicitly disable Kconfig by setting
# the variable to empty
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include

########################### COMPILE TIME CONFIGURATION ########################
# NOTE: The following options can be configured on runtime as well using
# `ifconfig`

# Change to use ABP by default
USE_ABP ?= 0

# The configurations can be accessed via menuconfig using `make menuconfig`

# OTAA compile time configuration keys

APP_KEY ?= AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
NWK_KEY ?= AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
APP_EUI ?= AAAAAAAAAAAAAAAA
JOIN_EUI ?= AAAAAAAAAAAAAAAA
DEV_EUI ?= AAAAAAAAAAAAAAAA

# ABP compile time configuration keys

DEV_ADDR ?= AAAAAAAA
NWK_SKEY ?= AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
APP_SKEY ?= AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

# Check if being configured via Kconfig
ifndef CONFIG_KCONFIG_USEMODULE_LORAWAN

  # static ABP configuration
  CFLAGS += -DCONFIG_LORAMAC_APP_KEY_DEFAULT=\"$(APP_KEY)\"
  CFLAGS += -DCONFIG_LORAMAC_NWK_KEY_DEFAULT=\"$(NWK_KEY)\"
  CFLAGS += -DCONFIG_LORAMAC_APP_EUI_DEFAULT=\"$(APP_EUI)\"
  CFLAGS += -DCONFIG_LORAMAC_JOIN_EUI_DEFAULT=\"$(JOIN_EUI)\"
  CFLAGS += -DCONFIG_LORAMAC_DEV_EUI_DEFAULT=\"$(DEV_EUI)\"

  # static OTAA configuration
  CFLAGS += -DCONFIG_LORAMAC_DEV_ADDR_DEFAULT=\"$(DEV_ADDR)\"
  CFLAGS += -DCONFIG_LORAMAC_NWK_SKEY_DEFAULT=\"$(NWK_SKEY)\"
  CFLAGS += -DCONFIG_LORAMAC_APP_SKEY_DEFAULT=\"$(APP_SKEY)\"

  ifeq ($(USE_ABP),0)
    CFLAGS += -DCONFIG_LORAMAC_DEFAULT_JOIN_PROCEDURE_OTAA
  else
    CFLAGS += -DCONFIG_LORAMAC_DEFAULT_JOIN_PROCEDURE_ABP
  endif

  # For TTN, It's necessary to set the RX2 DR to 3 in EU_868 region
  # CFLAGS += -DCONFIG_LORAMAC_DEFAULT_RX2_DR_3

  # For FIT-IoT Lab usage. Use the highest DR since gateway is nearby.
  # If uncommented, the default value (DR0) is used.
  # Note this value is also used for the OTAA.
  # CFLAGS += -DCONFIG_LORAMAC_DEFAULT_DR_5

  # Set default messages to unconfirmable
  CFLAGS += -DCONFIG_LORAMAC_DEFAULT_TX_MODE_UNCNF

  # Set region
  CFLAGS += -DCONFIG_LORAMAC_REGION_EU_868
endif

# We can reduce the size of the packet buffer for LoRaWAN, since there's no IP
# support. This will reduce RAM consumption.
# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=512
endif
