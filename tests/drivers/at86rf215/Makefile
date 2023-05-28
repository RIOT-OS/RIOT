INCLUDES += -I$(APPDIR)
BOARD ?= openmote-b

include ../Makefile.drivers_common

USEMODULE += test_utils_netdev_ieee802154_minimal

# the radio driver to test
USEMODULE += at86rf215
USEMODULE += at86rf215_batmon
USEMODULE += netdev_ieee802154_rx_timestamp

CFLAGS += -DEVENT_THREAD_STACKSIZE_DEFAULT=1024

include $(RIOTBASE)/Makefile.include
