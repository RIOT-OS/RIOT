MODULE = cpu

DIRS = $(RIOTCPU)/cortexm_common periph

SRC += cpu.c
SRC += vectors/vectors_$(CPU_MODEL).c

ifneq (,$(filter psoc6_ble_nimble,$(USEMODULE)))
  DIRS += psoc6_ble_nimble
endif

include $(RIOTBASE)/Makefile.base
