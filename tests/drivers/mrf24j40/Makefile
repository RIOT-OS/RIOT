INCLUDES += -I$(APPDIR)

include ../Makefile.drivers_common

USEMODULE += test_utils_netdev_ieee802154_minimal

# select the driver to test
USEMODULE += mrf24j40ma

CFLAGS += -DEVENT_THREAD_STACKSIZE_DEFAULT=1024

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter bhp,$(USEMODULE)))
  USEMODULE += bhp_event
endif
