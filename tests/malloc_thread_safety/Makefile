include ../Makefile.tests_common

USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include

# Only newlib and picolib provide mallinfo
ifeq (,$(filter newlib picolibc,$(USEMODULE)))
  CFLAGS += -DNO_MALLINFO
endif
