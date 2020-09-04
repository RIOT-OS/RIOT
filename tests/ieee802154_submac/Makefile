include ../Makefile.tests_common

BOARD_WHITELIST :=  \
					adafruit-clue \
					adafruit-itsybitsy-nrf52 \
					arduino-nano-33-ble \
					cc2538dk \
					feather-nrf52840 \
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
USEMODULE += ps
USEMODULE += event_thread_highest
USEMODULE += netdev_ieee802154_submac
USEMODULE += netdev_default

CFLAGS += -DEVENT_THREAD_HIGHEST_STACKSIZE=1024

include $(RIOTBASE)/Makefile.include
