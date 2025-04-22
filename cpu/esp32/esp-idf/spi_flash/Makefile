MODULE = esp_idf_spi_flash

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/esp_rom/patches/esp_rom_spiflash.c \
  components/hal/spi_flash_encrypt_hal_iram.c \
  components/hal/spi_flash_hal.c \
  components/hal/spi_flash_hal_iram.c \
  components/spi_flash/esp_flash_api.c \
  components/spi_flash/esp_flash_spi_init.c \
  components/spi_flash/flash_mmap.c \
  components/spi_flash/memspi_host_driver.c \
  components/spi_flash/spi_flash_os_func_app.c \
  components/spi_flash/spi_flash_os_func_noos.c \
  components/spi_flash/spi_flash_chip_boya.c \
  components/spi_flash/spi_flash_chip_drivers.c \
  components/spi_flash/spi_flash_chip_gd.c \
  components/spi_flash/spi_flash_chip_generic.c \
  components/spi_flash/spi_flash_chip_issi.c \
  components/spi_flash/spi_flash_chip_mxic.c \
  components/spi_flash/spi_flash_chip_mxic_opi.c \
  components/spi_flash/spi_flash_chip_th.c \
  components/spi_flash/spi_flash_chip_winbond.c \
  #

ifneq (,$(filter esp32s2 esp32s3 esp32c3 esp32c6 esp32h2,$(CPU_FAM)))
  ESP32_SDK_SRC += components/hal/spi_flash_hal_gpspi.c
endif

ifeq (,$(filter periph_spi,$(USEMODULE)))
  # no need to compile it here if it is already compiled for periph_spi
  ESP32_SDK_SRC += components/soc/$(CPU_FAM)/spi_periph.c
endif

# additional include pathes required by this module
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_rom/$(CPU_FAM)
INCLUDES += -I$(ESP32_SDK_DIR)/components/spi_flash/include/spi_flash

ifeq (,$(filter esp_spi_ram,$(USEMODULE)))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_psram/include
endif

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
