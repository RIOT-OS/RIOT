MODULE = esp_idf_gpio

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/esp_driver_gpio/src/gpio.c \
  components/esp_driver_gpio/src/rtc_io.c \
  components/esp_hw_support/rtc_module.c \
  components/hal/gpio_hal.c \
  components/hal/rtc_io_hal.c \
  components/soc/$(CPU_FAM)/gpio_periph.c \
  #

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
