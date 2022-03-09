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
USEMODULE += od
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += event_thread
USEMODULE += netdev_default
USEMODULE += luid
USEMODULE += l2util
USEMODULE += eui_provider
USEMODULE += ieee802154
USEMODULE += ieee802154_submac
USEMODULE += ztimer_usec

ifeq ($(BOARD), native)
  ZEP_PORT_BASE ?= 17754
  TERMFLAGS += -z [::1]:$(ZEP_PORT_BASE)
  USEMODULE += socket_zep
endif

CFLAGS += -DEVENT_THREAD_MEDIUM_STACKSIZE=1024

include $(RIOTBASE)/Makefile.include
include $(RIOTMAKE)/default-radio-settings.inc.mk
