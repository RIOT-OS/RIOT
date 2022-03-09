include ../Makefile.tests_common

BOARD_WHITELIST :=  \
					adafruit-clue \
					adafruit-itsybitsy-nrf52 \
					arduino-nano-33-ble \
					cc2538dk \
					feather-nrf52840 \
					native \
					nrf52840dk \
					nrf52840dongle \
					nrf52840-mdk \
					omote \
					openmote-cc2538 \
					reel \
					remote-pa \
					remote-reva \
					remote-revb \
					#

DISABLE_MODULE += auto_init_at86rf2xx auto_init_nrf802154

ifeq ($(BOARD), native)
  ZEP_PORT_BASE ?= 17754
  TERMFLAGS += -z [::1]:$(ZEP_PORT_BASE)
  USEMODULE += socket_zep
  # the same for Kconfig
  ifeq (1,$(TEST_KCONFIG))
    KCONFIG_ADD_CONFIG += $(APPDIR)/app.config.test.native
  endif
endif

USEMODULE += od
USEMODULE += luid
USEMODULE += l2util
USEMODULE += ieee802154
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += event_thread
USEMODULE += event_callback
USEMODULE += xtimer
USEMODULE += netdev_default

CFLAGS += -DEVENT_THREAD_MEDIUM_STACKSIZE=1024

include $(RIOTBASE)/Makefile.include
