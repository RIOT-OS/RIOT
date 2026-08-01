# only needed for SPI based variants
ifeq (,$(filter dali_host,$(USEMODULE)))
  SRC := $(filter-out dali_host.c,$(wildcard *.c))
endif
ifeq (,$(filter dali_peripheral,$(USEMODULE)))
  SRC := $(filter-out dali_peripheral.c,$(wildcard *.c))
endif



include $(RIOTBASE)/Makefile.base
