include ../Makefile.sys_common

USEMODULE += shell
USEMODULE += timex

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

ifeq (native, $(BOARD))
  RIOT_TERMINAL ?= native
endif

include $(RIOTBASE)/Makefile.include
