# additional include pathes required by this module
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/esp_supplicant/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/esp_supplicant/src
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/port/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/src
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/src/utils
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/src/crypto

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

# definitions for wpa_supplicant from components/wpa_supplicant/CMakeList.txt
CFLAGS += -D__ets__
CFLAGS += -DCONFIG_CRYPTO_INTERNAL
CFLAGS += -DCONFIG_ECC
CFLAGS += -DCONFIG_FAST_PBKDF2
CFLAGS += -DCONFIG_GMAC
CFLAGS += -DCONFIG_IEEE80211W
CFLAGS += -DCONFIG_NO_RADIUS
CFLAGS += -DCONFIG_SHA256
CFLAGS += -DCONFIG_TLSV11
CFLAGS += -DCONFIG_TLSV12
CFLAGS += -DCONFIG_TLS_INTERNAL_CLIENT
CFLAGS += -DCONFIG_WPS
CFLAGS += -DEAP_FAST
CFLAGS += -DEAP_MSCHAPv2
CFLAGS += -DEAP_PEAP
CFLAGS += -DEAP_PEER_METHOD
CFLAGS += -DEAP_TLS
CFLAGS += -DEAP_TTLS
CFLAGS += -DESPRESSIF_USE
CFLAGS += -DESP_PLATFORM
CFLAGS += -DESP_SUPPLICANT
CFLAGS += -DIEEE8021X_EAPOL
CFLAGS += -DUSE_WPA2_TASK
CFLAGS += -DUSE_WPS_TASK

# additional compiler flags to avoid compile errors
CFLAGS += -Wno-strict-aliasing
CFLAGS += -Wno-format-nonliteral
CFLAGS += -Wno-format-security
CFLAGS += -std=gnu99

include $(ESP_IDF_PATH)/esp_idf.mk
include $(ESP_IDF_PATH)/esp_idf_cflags.mk
