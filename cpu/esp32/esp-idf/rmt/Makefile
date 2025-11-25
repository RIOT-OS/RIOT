MODULE = esp_idf_rmt

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/driver/rmt.c \
  components/soc/$(CPU_FAM)/rmt_periph.c \
  components/hal/rmt_hal.c \
  #

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
