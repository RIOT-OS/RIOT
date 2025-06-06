MODULE = esp_idf_adc

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/esp_adc/adc_cali.c \
  components/esp_adc/adc_common.c \
  components/esp_adc/adc_oneshot.c \
  components/esp_hw_support/adc_share_hw_ctrl.c \
  components/hal/adc_hal_common.c \
  components/hal/adc_oneshot_hal.c \
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

ifneq (,$(filter esp32 esp32s2,$(CPU_FAM)))
  ESP32_SDK_SRC += components/esp_adc/$(CPU_FAM)/adc_cali_line_fitting.c
else
  ESP32_SDK_SRC += components/esp_adc/adc_cali_curve_fitting.c
endif

ifneq (,$(filter esp32h2 esp32s3 esp32c3 esp32c6,$(CPU_FAM)))
  ESP32_SDK_SRC += components/esp_adc/$(CPU_FAM)/curve_fitting_coefficients.c
endif

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

CFLAGS += -Wno-unused-but-set-variable

INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_adc/interface

include ../esp_idf.mk
include ../esp_idf_cflags.mk
