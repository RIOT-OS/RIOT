# name of your application
APPLICATION = gnrc_lorawan

# Turn on developer helpers
DEVELHELP ?= 1

BOARD ?= b-l072z-lrwan1
RIOTBASE ?= ../../

# Include board's default network devices and auto-initialization of GNRC
# interfaces
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif

# Add support for GNRC LoRaWAN
USEMODULE += gnrc_lorawan

# Use GNRC pktdump to print downlink messages
USEMODULE += gnrc_pktdump

# Include the shell and shell commands.
USEMODULE += shell
USEMODULE += shell_commands

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

# The configurations can be accessed via menuconfig using `make menuconfig`

# OTAA compile time configuration keys

# Check if being configured via Kconfig
ifndef CONFIG_KCONFIG_USEMODULE_LORAWAN

  # Tell GNRC to encode LoRaWAN port in the GNRC netif header.
  # This allows us to use `gnrc_txtsnd` to send data from the shell using the
  # `txtsnd` command.
  #
  # Note: From Release 22.01 all GNRC LoRaWAN packets will include the netif
  # header. Therefore this flag will be removed after that
  CFLAGS += -DCONFIG_GNRC_NETIF_LORAWAN_NETIF_HDR

  CFLAGS += -DCONFIG_LORAMAC_APP_KEY_DEFAULT=\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\"
  CFLAGS += -DCONFIG_LORAMAC_APP_EUI_DEFAULT=\"BBBBBBBBBBBBBBBB\"
  CFLAGS += -DCONFIG_LORAMAC_DEV_EUI_DEFAULT=\"CCCCCCCCCCCCCCCC\"

  # For TTN, It's necessary to set the RX2 DR to 3 in EU_868 region
  # CFLAGS += -DCONFIG_LORAMAC_DEFAULT_RX2_DR_3

  # Comment/uncomment as necessary
  CFLAGS += -DCONFIG_LORAMAC_DEFAULT_JOIN_PROCEDURE_OTAA
  # CFLAGS += -DCONFIG_LORAMAC_DEFAULT_JOIN_PROCEDURE_ABP

  # Uncomment and replace with proper keys for joining with ABP
  # NOTE: This values will be overriten in case of OTAA.
  # CFLAGS += -DCONFIG_LORAMAC_DEV_ADDR_DEFAULT=\"00000000\"
  # CFLAGS += -DCONFIG_LORAMAC_NWK_SKEY_DEFAULT=\"00000000000000000000000000000000\"
  # CFLAGS += -DCONFIG_LORAMAC_APP_SKEY_DEFAULT=\"00000000000000000000000000000000\"

  ## For FIT-IoT Lab usage. Use the highest DR since gateway is nearby.
  # If uncommented, the default value (DR0) is used.
  # Note this value is also used for the OTAA.
  # CFLAGS += -DCONFIG_LORAMAC_DEFAULT_DR_5

  # # Set default messages to unconfirmable
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
