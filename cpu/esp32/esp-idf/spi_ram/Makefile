MODULE = esp_idf_spi_ram

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/esp_hw_support/port/$(CPU)/spiram.c \
  components/esp_hw_support/port/$(CPU)/spiram_psram.c \
  components/esp_hw_support/port/$(CPU)/cache_sram_mmu.c \
  #

# additional include pathes required by this module
INCLUDES += -I$(ESP32_SDK_DIR)/components/soc/$(CPU)/include

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
