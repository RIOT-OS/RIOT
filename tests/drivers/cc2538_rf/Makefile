INCLUDES += -I$(APPDIR)
BOARD ?= remote-revb

include ../Makefile.drivers_common

USEMODULE += test_utils_netdev_ieee802154_minimal

# select the driver to test
USEMODULE += cc2538_rf

FEATURES_REQUIRED += cpu_cc2538

CFLAGS += -DEVENT_THREAD_STACKSIZE_DEFAULT=1024

include $(RIOTBASE)/Makefile.include
