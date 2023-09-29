MODULE = sdmmc

SRC = sdmmc.c

ifneq (,$(filter sdmmc_sdhc,$(USEMODULE)))
  SRC += sdmmc_sdhc.c
endif

include $(RIOTBASE)/Makefile.base
