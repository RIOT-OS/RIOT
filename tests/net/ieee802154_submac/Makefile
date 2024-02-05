include ../Makefile.net_common
#
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

USEMODULE += od
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += event_thread
USEMODULE += netdev_default
USEMODULE += luid
USEMODULE += l2util
USEMODULE += eui_provider
USEMODULE += ieee802154
USEMODULE += ieee802154_submac
USEMODULE += ztimer_usec

ifneq (,$(filter native native64,$(BOARD)))
  USE_ZEP = 1
  USEMODULE += socket_zep
endif

CFLAGS += -DEVENT_THREAD_MEDIUM_STACKSIZE=1024

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter bhp,$(USEMODULE)))
  USEMODULE += bhp_event
endif

include $(RIOTMAKE)/default-radio-settings.inc.mk
