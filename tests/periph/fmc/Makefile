BOARD ?= stm32f429i-disc1
include ../Makefile.periph_common

USEMODULE += benchmark
USEMODULE += od

FEATURES_REQUIRED += periph_fmc
FEATURES_OPTIONAL += periph_fmc_nor_sram
FEATURES_OPTIONAL += periph_fmc_sdram

include $(RIOTBASE)/Makefile.include
