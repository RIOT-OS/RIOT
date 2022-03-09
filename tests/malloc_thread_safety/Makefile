include ../Makefile.tests_common

PICOLIBC ?= 0
ifneq (0,$(PICOLIBC))
  FEATURES_REQUIRED += picolibc
endif
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include

# Only newlib and picolib provide mallinfo
ifeq (,$(filter newlib picolibc,$(USEMODULE)))
  CFLAGS += -DNO_MALLINFO
endif
