MODULE = esp_idf_lcd

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/esp_lcd/src/esp_lcd_common.c \
  components/esp_lcd/src/esp_lcd_panel_io.c \
  components/esp_pm/pm_locks.c \
  components/soc/$(CPU_FAM)/lcd_periph.c \
  #

ifeq (esp32s3,$(CPU_FAM))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/dma/include
  ESP32_SDK_SRC += components/esp_lcd/i80/esp_lcd_panel_io_i80.c
  ESP32_SDK_SRC += components/hal/lcd_hal.c
  ifeq (,$(filter esp_idf_rmt,$(USEMODULE)))
    ESP32_SDK_SRC += components/bootloader_support/src/flash_encrypt.c
    ESP32_SDK_SRC += components/esp_hw_support/dma/gdma.c
    ESP32_SDK_SRC += components/esp_hw_support/dma/gdma_link.c
    ESP32_SDK_SRC += components/hal/gdma_hal_ahb_v1.c
    ESP32_SDK_SRC += components/hal/gdma_hal_top.c
    ESP32_SDK_SRC += components/soc/$(CPU_FAM)/gdma_periph.c
    ESP32_SDK_ASMSRC += components/esp_rom/patches/esp_rom_cache_writeback_esp32s3.S
  endif
  ifeq (,$(filter esp_idf_sdmmc,$(USEMODULE)))
    ESP32_SDK_SRC += components/esp_hw_support/esp_memory_utils.c
  endif
else ifneq (,$(filter esp32 esp32s2,$(CPU_FAM)))
  ESP32_SDK_SRC = \
    components/driver/i2s.c \
    components/esp_lcd/src/esp_lcd_panel_io_i2s.c \
    components/hal/i2s_hal.c \
    #
endif

# additional include pathes required by this module
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_lcd/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_lcd/interface
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_lcd/priv_include

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
