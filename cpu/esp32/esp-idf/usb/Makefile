MODULE = esp_idf_usb

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/hal/usb_hal.c \
  components/hal/usb_phy_hal.c \
  components/soc/$(CPU_FAM)/usb_periph.c \
  components/soc/$(CPU_FAM)/usb_phy_periph.c \
  components/usb/usb_phy.c \
  #

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
