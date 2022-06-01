BOARD ?= openmote-b

# the radio driver to test
USEMODULE += at86rf215
USEMODULE += at86rf215_batmon
USEMODULE += netdev_ieee802154_rx_timestamp

CFLAGS += -DCONFIG_NETDEV_REGISTER_SIGNAL

include ../driver_netdev_common/Makefile.netdev.mk
