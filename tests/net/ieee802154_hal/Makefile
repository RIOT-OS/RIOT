include ../Makefile.net_common

ifneq (,$(filter native native32 native64,$(BOARD)))
  USE_ZEP := 1
  ZEP_PORT_BASE ?= 17754
  USEMODULE += socket_zep
else
  # on non-native boards, we need to bump the event thread's stack
  CFLAGS += -DEVENT_THREAD_MEDIUM_STACKSIZE=1024
endif

USEMODULE += od
USEMODULE += luid
USEMODULE += l2util
USEMODULE += ieee802154
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += event_thread
USEMODULE += event_callback
USEMODULE += xtimer
USEMODULE += netdev_default

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter bhp,$(USEMODULE)))
  USEMODULE += bhp_event
endif
