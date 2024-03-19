SUBMODULES := 1

SRC := kw2xrf.c kw2xrf_getset.c kw2xrf_intern.c kw2xrf_radio_hal.c kw2xrf_spi.c

ifneq (,$(filter kw2xrf_testmode,$(USEMODULE)))
  SRC += kw2xrf_tm.c
endif

include $(RIOTBASE)/Makefile.base
