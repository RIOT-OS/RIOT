BOARD ?= openmote-b

# the radio driver to test
USEMODULE += at86rf215
USEMODULE += at86rf215_batmon
USEMODULE += at86rf215_timestamp

CFLAGS += -DCONFIG_NETDEV_REGISTER_SIGNAL

include ../driver_netdev_common/Makefile.netdev.mk
