include ../Makefile.net_common

# Due to missing Kconfig support, whitelist boards that are known to have a
# working Kconfig dependency resolution.
# Other boards should still work when running with CONTINUE_ON_EXPECTED_ERRORS=1

BOARDS_SUPPORTED += adafruit-clue \
                   adafruit-itsybitsy-nrf52 \
                   arduino-nano-33-ble \
                   atmega256rfr2-xpro \
                   avr-rss2 \
                   cc2538dk \
                   derfmega128 \
                   derfmega256 \
                   esp32c6-devkit \
                   esp32h2-devkit \
                   feather-nrf52840 \
                   frdm-kw41z \
                   microduino-corerf \
                   native \
                   nrf52840dk \
                   nrf52840dongle \
                   nrf52840-mdk \
                   nucleo-l152re \
                   nucleo-f303re \
                   nucleo-wl55jc \
                   omote \
                   openlabs-kw41z-mini \
                   openmote-cc2538 \
                   pba-d-01-kw2x \
                   phynode-kw41z \
                   reel \
                   remote-pa \
                   remote-reva \
                   remote-revb \
                   samr21-xpro \
                   samr30-xpro \
                   seeedstudio-xiao-esp32c6 \
                   usb-kw41z \
                   zigduino \
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
USEMODULE += ztimer_msec

ifneq (,$(filter native native32 native64,$(BOARD)))
  USE_ZEP = 1
  USEMODULE += socket_zep
endif

CFLAGS += -DEVENT_THREAD_MEDIUM_STACKSIZE=1024
include $(RIOTBASE)/Makefile.include

ifneq (,$(filter bhp,$(USEMODULE)))
  USEMODULE += bhp_event
endif

include $(RIOTMAKE)/default-radio-settings.inc.mk
