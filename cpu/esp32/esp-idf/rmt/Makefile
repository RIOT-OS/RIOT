MODULE = esp_idf_rmt

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/esp_driver_rmt/src/rmt_common.c \
  components/esp_driver_rmt/src/rmt_encoder.c \
  components/esp_driver_rmt/src/rmt_tx.c \
  components/esp_pm/pm_locks.c \
  components/hal/rmt_hal.c \
  components/soc/$(CPU_FAM)/rmt_periph.c \
  #

ifeq (esp32s3,$(CPU_FAM))
  ESP32_SDK_SRC += components/bootloader_support/src/flash_encrypt.c
  ESP32_SDK_SRC += components/esp_hw_support/dma/gdma.c
  ESP32_SDK_SRC += components/hal/gdma_hal_ahb_v1.c
  ESP32_SDK_SRC += components/hal/gdma_hal_top.c
  ESP32_SDK_SRC += components/soc/$(CPU_FAM)/gdma_periph.c
  ESP32_SDK_ASMSRC += components/esp_rom/patches/esp_rom_cache_writeback_esp32s3.S
endif

INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/dma/include

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
