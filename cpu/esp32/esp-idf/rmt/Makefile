MODULE = esp_idf_rmt

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/esp_driver_rmt/src/rmt_common.c \
  components/esp_driver_rmt/src/rmt_encoder.c \
  components/esp_driver_rmt/src/rmt_encoder_simple.c \
  components/esp_driver_rmt/src/rmt_tx.c \
  components/esp_pm/pm_locks.c \
  components/esp_hal_rmt/rmt_hal.c \
  components/esp_hal_rmt/$(CPU_FAM)/rmt_periph.c \
  #

ifeq (esp32s3,$(CPU_FAM))
  ifeq (,$(filter esp_idf_nvs_flash,$(USEMODULE)))
    # this file is already compiled in `esp_idf_nvs_flash`
    # don't compile it again if `esp_idf_nvs_flash` is used
    ESP32_SDK_SRC += components/bootloader_support/src/flash_encrypt.c
  endif
  ESP32_SDK_SRC += components/esp_driver_dma/src/gdma.c
  ESP32_SDK_SRC += components/esp_hal_dma/gdma_hal_ahb_v1.c
  ESP32_SDK_SRC += components/esp_hal_dma/gdma_hal_top.c
  ESP32_SDK_SRC += components/esp_hal_dma/$(CPU_FAM)/gdma_periph.c
endif

INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_driver_dma/include

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
