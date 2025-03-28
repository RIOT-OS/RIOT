MODULE = esp_idf_spi_ram

# source files to be compiled for this module
ESP32_SDK_SRC += components/esp_psram/esp_psram.c

ifeq (esp32,$(CPU_FAM))
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/cache_sram_mmu.c
  ESP32_SDK_SRC += components/esp_psram/$(CPU_FAM)/esp_psram_extram_cache.c
  ESP32_SDK_SRC += components/esp_psram/$(CPU_FAM)/esp_psram_impl_quad.c
endif

ifeq (esp32s2,$(CPU_FAM))
  ESP32_SDK_SRC += components/esp_psram/$(CPU_FAM)/esp_psram_impl_quad.c
endif

ifeq (esp32s3,$(CPU_FAM))
  ifneq (,$(filter esp_spi_oct,$(USEMODULE)))
    ESP32_SDK_SRC += components/esp_psram/$(CPU_FAM)/esp_psram_impl_octal.c
  else
    ESP32_SDK_SRC += components/esp_psram/device/esp_psram_impl_ap_quad.c
  endif
endif

CFLAGS += -Wno-unused-but-set-variable

# additional include pathes required by this module
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_driver_spi/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_psram/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/soc/$(CPU_FAM)/include

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
