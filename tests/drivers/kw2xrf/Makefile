INCLUDES += -I$(APPDIR)
BOARD ?= pba-d-01-kw2x

include ../Makefile.drivers_common

BOARDS_SUPPORTED :=  \
					pba-d-01-kw2x \
					#

USEMODULE += test_utils_netdev_ieee802154_minimal

# the radio driver to test
USEMODULE += kw2xrf
USEMODULE += kw2xrf_testmode

CFLAGS += -DEVENT_THREAD_STACKSIZE_DEFAULT=1024

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter bhp,$(USEMODULE)))
  USEMODULE += bhp_event
endif
