INCLUDES += -I$(APPDIR)
BOARD ?= z1

include ../Makefile.drivers_common

USEMODULE += test_utils_netdev_ieee802154_minimal

# select the driver to test
USEMODULE += cc2420

CFLAGS += -DEVENT_THREAD_STACKSIZE_DEFAULT=1024

include $(RIOTBASE)/Makefile.include
