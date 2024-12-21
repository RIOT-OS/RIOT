include ../Makefile.net_common

ifneq (,$(filter native native32,$(BOARD)))
  ZEP_PORT_BASE ?= 17754
  TERMFLAGS += -z [::1]:$(ZEP_PORT_BASE)
  USEMODULE += socket_zep
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

CFLAGS += -DEVENT_THREAD_MEDIUM_STACKSIZE=1024

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter bhp,$(USEMODULE)))
  USEMODULE += bhp_event
endif
