MODULE = sdmmc

MODULE_DEFS_NEEDED += \
  cortexm_common \
  periph_sdmmc_auto_clk \
  periph_sdmmc_auto_cmd12 \
  periph_sdmmc_hs \
  sdmmc_mmc \
  sdmmc_sdio \
  ztimer_msec \
  ztimer_usec \
  #

SRC = sdmmc.c

ifneq (,$(filter sdmmc_sdhc,$(USEMODULE)))
  SRC += sdmmc_sdhc.c
endif

include $(RIOTBASE)/Makefile.base
