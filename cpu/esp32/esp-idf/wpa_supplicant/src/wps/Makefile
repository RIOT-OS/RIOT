MODULE = esp_idf_wpa_supplicant_wps

# source file list to be compiled as configured in component.mk
ESP32_SDK_SRC = \
  components/wpa_supplicant/src/wps/wps_attr_build.c \
  components/wpa_supplicant/src/wps/wps_attr_parse.c \
  components/wpa_supplicant/src/wps/wps_attr_process.c \
  components/wpa_supplicant/src/wps/wps.c \
  components/wpa_supplicant/src/wps/wps_common.c \
  components/wpa_supplicant/src/wps/wps_dev_attr.c \
  components/wpa_supplicant/src/wps/wps_enrollee.c \
  #

include $(ESP_IDF_WPA_SUPPLICANT_PATH)/esp_idf_wpa_supplicant.mk

CFLAGS += -Wno-format
