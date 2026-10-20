MODULE = esp_idf_sdmmc

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/esp_driver_sdmmc/legacy/src/sdmmc_host.c \
  components/esp_driver_sdmmc/legacy/src/sdmmc_transaction.c \
  components/esp_driver_sdmmc/src/sd_host_sdmmc.c \
  components/esp_driver_sdmmc/src/sd_trans_sdmmc.c \
  components/esp_driver_sd_intf/sd_host.c \
  components/esp_hal_sd/$(CPU_FAM)/sdmmc_periph.c \
  components/esp_hal_sd/sdmmc_hal.c \
  #

# additional include pathes required by this module
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_driver_dma/include

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
