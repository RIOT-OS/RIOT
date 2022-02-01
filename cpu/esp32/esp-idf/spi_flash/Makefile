MODULE = esp_idf_spi_flash

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/bootloader_support/src/bootloader_common.c \
  components/bootloader_support/src/bootloader_efuse_$(CPU).c \
  components/bootloader_support/src/bootloader_flash_config_$(CPU).c \
  components/driver/spi_common.c \
  components/soc/$(CPU)/spi_periph.c \
  components/spi_flash/$(CPU)/spi_flash_rom_patch.c \
  components/spi_flash/flash_ops.c \
  #

# additional include pathes required by this module
INCLUDES += -I$(ESP32_SDK_DIR)/components/bootloader_support/include_bootloader
INCLUDES += -I$(ESP32_SDK_DIR)/components/spi_flash/include/spi_flash

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
