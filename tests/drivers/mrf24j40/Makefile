INCLUDES += -I$(APPDIR)

include ../Makefile.drivers_common

# Due to missing Kconfig support, whitelist boards that are known to have a
# working Kconfig dependency resolution.
# Other boards should still work when running with CONTINUE_ON_EXPECTED_ERRORS=1

BOARD_WHITELIST += adafruit-clue \
                   adafruit-itsybitsy-nrf52 \
                   arduino-nano-33-ble \
                   cc2538dk \
                   feather-nrf52840 \
                   native \
                   native64 \
                   nrf52840dk \
                   nrf52840dongle \
                   nrf52840-mdk \
                   nucleo-l152re \
                   nucleo-f303re \
                   nucleo-wl55jc \
                   omote \
                   openmote-cc2538 \
                   reel \
                   remote-pa \
                   remote-reva \
                   remote-revb \
                   pba-d-01-kw2x \
                   #

USEMODULE += test_utils_netdev_ieee802154_minimal

# select the driver to test
USEMODULE += mrf24j40ma

CFLAGS += -DEVENT_THREAD_STACKSIZE_DEFAULT=1024

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter bhp,$(USEMODULE)))
  USEMODULE += bhp_event
endif
