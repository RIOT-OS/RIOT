INCLUDES += -I$(APPDIR)
BOARD ?= nrf52840dk

include ../Makefile.drivers_common

USEMODULE += test_utils_netdev_ieee802154_minimal

# select the driver to test
USEMODULE += nrf802154

FEATURES_REQUIRED += cpu_nrf52

CFLAGS += -DEVENT_THREAD_STACKSIZE_DEFAULT=1024

include $(RIOTBASE)/Makefile.include
