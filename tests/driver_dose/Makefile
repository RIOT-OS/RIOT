# the driver to test
USEMODULE += dose

# compile-test dose_watchdog on select boards
ifneq (,$(filter samr21-xpro, $(BOARD)))
  # xtimer uses timer 1 on this board
  CFLAGS += -DDOSE_TIMER_DEV=TIMER_DEV\(0\)
  USEMODULE += dose_watchdog
endif

ifneq (,$(filter same54-xpro, $(BOARD)))
  CFLAGS += -DDOSE_TIMER_DEV=TIMER_DEV\(1\)
  USEMODULE += dose_watchdog
endif

include ../driver_netdev_common/Makefile.netdev.mk
