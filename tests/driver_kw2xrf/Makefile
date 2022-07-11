INCLUDES += -I$(APPDIR)
BOARD ?= pba-d-01-kw2x

include ../Makefile.tests_common

USEMODULE += test_utils_netdev_ieee802154_minimal

# the radio driver to test
USEMODULE += kw2xrf

CFLAGS += -DEVENT_THREAD_STACKSIZE_DEFAULT=1024

include $(RIOTBASE)/Makefile.include
