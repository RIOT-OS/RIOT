MODULE = esp_idf_adc

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/driver/deprecated/adc_legacy.c \
  components/esp_hw_support/adc_share_hw_ctrl.c \
  components/hal/adc_hal_common.c \
  components/soc/$(CPU_FAM)/adc_periph.c \
  #

ifeq (,$(filter esp_idf_gpio,$(USEMODULE)))
  ESP32_SDK_SRC += components/hal/rtc_io_hal.c
endif

ifneq (esp32,$(CPU_FAM))
  ESP32_SDK_SRC += components/efuse/$(CPU_FAM)/esp_efuse_rtc_calib.c
endif

ifeq (esp32s2,$(CPU_FAM))
  ESP32_SDK_SRC += components/efuse/esp32s2/esp_efuse_rtc_table.c
endif

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

INCLUDES += -I$(ESP32_SDK_DIR)/components/driver/deprecated

include ../esp_idf.mk
include ../esp_idf_cflags.mk
