MODULE = cpu

DIRS = $(RIOTCPU)/msp430_common periph

# cc430_rf radio driver
ifneq (,$(filter cc430_rf,$(USEMODULE)))
  DIRS += radio gnrc_cc110x
endif

include $(RIOTBASE)/Makefile.base
