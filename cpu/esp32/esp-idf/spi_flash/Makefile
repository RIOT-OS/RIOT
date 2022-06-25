MODULE = esp_idf_spi_flash

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/bootloader_support/src/bootloader_common.c \
  components/driver/spi_common.c \
  components/spi_flash/$(CPU)/flash_ops_$(CPU).c \
  components/spi_flash/$(CPU)/spi_flash_rom_patch.c \
  components/spi_flash/esp_flash_api.c \
  components/spi_flash/partition.c \
  #

ifeq (,$(filter periph_spi,$(USEMODULE)))
  # no need to compile it here if it is already compiled for periph_spi
  ESP32_SDK_SRC += components/soc/$(CPU)/spi_periph.c
endif

ifneq (,$(filter esp32,$(CPU)))
  ESP32_SDK_SRC += components/spi_flash/flash_mmap.c
endif

# additional include pathes required by this module
INCLUDES += -I$(ESP32_SDK_DIR)/components/app_update/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/bootloader_support/include_bootloader
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_rom/$(CPU)
INCLUDES += -I$(ESP32_SDK_DIR)/components/spi_flash/include/spi_flash

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
