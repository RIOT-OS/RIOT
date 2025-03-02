MODULE = esp_idf_wifi

ESP32_SDK_SRC = \
  components/esp_coex/$(CPU_FAM)/esp_coex_adapter.c \
  components/esp_phy/$(CPU_FAM)/phy_init_data.c \
  components/esp_phy/src/phy_common.c \
  components/esp_phy/src/phy_init.c \
  components/esp_system/port/soc/$(CPU_FAM)/reset_reason.c \
  components/esp_wifi/$(CPU_FAM)/esp_adapter.c \
  components/esp_wifi/src/wifi_init.c \
  components/soc/esp32/dport_access.c \
  #

# additional include pathes required by this module
INCLUDES += -I$(ESP32_SDK_DIR)/components/app_update/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_bootloader_format/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_coex/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_partition/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_phy/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_phy/$(CPU_FAM)/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/port/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/esp_supplicant/include

ifneq (,$(filter esp32c3 esp32s2,$(CPU_FAM)))
  ESP32_SDK_SRC += components/esp_hw_support/port/$(CPU_FAM)/adc2_init_cal.c
endif

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
