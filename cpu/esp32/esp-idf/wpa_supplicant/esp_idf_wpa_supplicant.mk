# additional include pathes required by this module
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/esp_supplicant/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/esp_supplicant/src
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/port/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/src
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/src/utils

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

# definitions for wpa_supplicant from components/wpa_supplicant/component.mk
CFLAGS += -D__ets__
CFLAGS += -DCONFIG_DPP
CFLAGS += -DCONFIG_ECC
CFLAGS += -DCONFIG_IEEE80211W
CFLAGS += -DCONFIG_SHA256
CFLAGS += -DCONFIG_WNM
CFLAGS += -DCONFIG_WPS_PIN
CFLAGS += -DCONFIG_WPS2
CFLAGS += -DEAP_MSCHAPv2
CFLAGS += -DEAP_PEAP
CFLAGS += -DEAP_PEER_METHOD
CFLAGS += -DEAP_TLS
CFLAGS += -DEAP_TTLS
CFLAGS += -DESP_SUPPLICANT
CFLAGS += -DESP32_WORKAROUND
CFLAGS += -DESPRESSIF_USE
CFLAGS += -DIEEE8021X_EAPOL
CFLAGS += -DUSE_WPA2_TASK
CFLAGS += -DUSE_WPS_TASK
CFLAGS += -Wno-strict-aliasing
CFLAGS += -Wno-format-nonliteral
CFLAGS += -Wno-format-security
CFLAGS += -std=gnu99

include $(ESP_IDF_PATH)/esp_idf.mk
include $(ESP_IDF_PATH)/esp_idf_cflags.mk
