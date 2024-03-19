MODULE = esp_idf_wpa_supplicant

export ESP_IDF_WPA_SUPPLICANT_PATH = $(shell pwd)

DIRS += esp_supplicant
DIRS += port
DIRS += src/ap
DIRS += src/common
DIRS += src/crypto
DIRS += src/eap_peer
DIRS += src/rsn_supp
DIRS += src/tls
DIRS += src/utils
DIRS += src/wps

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)
