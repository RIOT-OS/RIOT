MODULE = esp_idf_spi_ram

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/esp_hw_support/port/$(CPU_FAM)/spiram.c \
  components/esp_hw_support/port/$(CPU_FAM)/spiram_psram.c \
  #

ifeq (esp32,$(CPU_FAM))
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/cache_sram_mmu.c
endif

ifneq (,$(filter esp_spi_oct,$(USEMODULE)))
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/opiram_psram.c
endif

# additional include pathes required by this module
INCLUDES += -I$(ESP32_SDK_DIR)/components/soc/$(CPU_FAM)/include

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
