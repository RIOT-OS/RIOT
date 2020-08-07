MODULE=esp_idf_wpa_supplicant_wpa2_utils

include $(RIOTBASE)/Makefile.base

# we have to do it in that way to avoid that $(RIOTBASE)/sys/include/crypto
# is found first
INCLUDES  = -I$(RIOTCPU)/$(CPU)/vendor/esp-idf/wpa_supplicant/include
INCLUDES += -I$(RIOTCPU)/$(CPU)/vendor/esp-idf/wpa_supplicant/include/wpa
INCLUDES += -I$(RIOTCPU)/$(CPU)/vendor/esp-idf/wpa_supplicant/port/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/port/include
CFLAGS += -D__ets__ -DESPRESSIF_USE -DESP32_IDF_CODE=1

include $(RIOTCPU)/$(CPU)/Makefile.include

INCLUDES += -I$(RIOTBASE)/core/include
INCLUDES += -I$(RIOTCPU)/$(CPU)/vendor/esp-idf/include/log
INCLUDES += -I$(RIOTCPU)/$(CPU)/include
