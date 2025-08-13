MODULE = esp_idf_sdmmc

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/esp_driver_sdmmc/src/sdmmc_host.c \
  components/esp_driver_sdmmc/src/sdmmc_transaction.c \
  components/soc/$(CPU_FAM)/sdmmc_periph.c \
  components/hal/sdmmc_hal.c \
  components/esp_hw_support/dma/esp_dma_utils.c \
  components/esp_hw_support/esp_memory_utils.c \
  #

# additional include pathes required by this module
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_driver_sdmmc/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/sdmmc/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/dma/include

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
