include ../Makefile.net_common

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

ifneq (,$(filter native native32,$(BOARD)))
  USE_ZEP = 1
  USEMODULE += socket_zep
endif

CFLAGS += -DEVENT_THREAD_MEDIUM_STACKSIZE=1024

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter bhp,$(USEMODULE)))
  USEMODULE += bhp_event
endif

include $(RIOTMAKE)/default-radio-settings.inc.mk
