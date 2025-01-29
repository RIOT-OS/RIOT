MODULE = esp_idf_adc

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/driver/adc.c \
  components/driver/adc_common.c \
  components/hal/adc_hal.c \
  components/soc/$(CPU_FAM)/adc_periph.c \
  #

ifneq (,$(filter esp32c3 esp32s2,$(CPU_FAM)))
  ESP32_SDK_SRC += components/driver/$(CPU_FAM)/adc2_init_cal.c
  INCLUDES += -I$(ESP32_SDK_DIR)/components/driver/include/driver
endif

ifneq (,$(filter esp32c3 esp32h2 esp32s3,$(CPU_FAM)))
  ESP32_SDK_SRC += components/efuse/$(CPU_FAM)/esp_efuse_rtc_calib.c \
									 components/driver/gdma.c \
									 components/soc/$(CPU_FAM)/gdma_periph.c \
									 components/hal/gdma_hal.c \
	#
endif

ifneq (,$(filter esp32s2,$(CPU_FAM)))
  ESP32_SDK_SRC += components/efuse/esp32s2/esp_efuse_rtc_table.c
endif

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
