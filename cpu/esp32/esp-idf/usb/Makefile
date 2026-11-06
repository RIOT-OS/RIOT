MODULE = esp_idf_usb

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/esp_hal_usb/usb_dwc_hal.c \
  components/esp_hal_usb/usb_wrap_hal.c \
  components/esp_hal_usb/$(CPU_FAM)/usb_dwc_periph.c \
  components/esp_hw_support/usb_phy/usb_phy.c \
  #

ifeq (esp32s3,$(CPU_FAM))
  ESP32_SDK_SRC += components/esp_hal_usb/usb_serial_jtag_hal.c
endif

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
