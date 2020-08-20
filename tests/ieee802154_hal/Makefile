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
					openmote-b \
					openmote-cc2538 \
					reel \
					remote-pa \
					remote-reva \
					remote-revb \
					#

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

CFLAGS += -DEVENT_THREAD_HIGHEST_STACKSIZE=1024

# define the driver to be used for selected boards
DRIVER := cc2538_rf

# include the selected driver
USEMODULE += $(DRIVER)

include $(RIOTBASE)/Makefile.include
