include ../Makefile.tests_common

USEMODULE += test_utils_netdev_eth_minimal

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

INCLUDES += -I$(APPDIR)

include $(RIOTBASE)/Makefile.include
