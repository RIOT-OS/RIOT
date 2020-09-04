include ../Makefile.tests_common

DISABLE_MODULE += auto_init_at86rf2xx auto_init_nrf802154

USEMODULE += od
USEMODULE += luid
USEMODULE += ieee802154
USEMODULE += shell
USEMODULE += ps
USEMODULE += event_thread_highest
USEMODULE += event_callback
USEMODULE += xtimer
USEMODULE += ieee802154_radio_hal
USEMODULE += ieee802154_submac
USEMODULE += netdev_ieee802154_submac

CFLAGS += -DEVENT_THREAD_HIGHEST_STACKSIZE=1024
CFLAGS += -DDEBUG_ASSERT_VERBOSE=1

NETDEV ?= 0

CFLAGS += -DNETDEV=$(NETDEV)

# define the driver to be used for selected boards
ifneq (,$(filter samr21-xpro,$(BOARD)))
  DRIVER := at86rf233
endif
ifneq (,$(filter iotlab-m3 fox,$(BOARD)))
  DRIVER := at86rf231
endif
ifneq (,$(filter nrf52840dk,$(BOARD)))
  DRIVER := nrf802154
endif
ifneq (,$(filter cc2538dk,$(BOARD)))
  DRIVER := cc2538_rf
endif

# use the at86rf231 as fallback device
DRIVER ?= at86rf231

# include the selected driver
USEMODULE += $(DRIVER)

include $(RIOTBASE)/Makefile.include
