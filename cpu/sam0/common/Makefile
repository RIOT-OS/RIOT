DIRS = periph

ifneq (,$(filter sam0_eth, $(USEMODULE)))
  DIRS += sam0_eth
endif
ifneq (,$(filter sam0_sdhc, $(USEMODULE)))
  DIRS += sam0_sdhc
endif

include $(RIOTBASE)/Makefile.base
