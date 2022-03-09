MODULE=esp_idf_wpa_supplicant_wpa2_eap_peer

include $(RIOTBASE)/Makefile.base

# we have to do it in that way to avoid that $(RIOTBASE)/sys/include/crypto
# is found first
PRE_INCLUDES += -I$(RIOTCPU)/$(CPU)/vendor/esp-idf/include/log
PRE_INCLUDES += -I$(RIOTCPU)/$(CPU)/vendor/esp-idf/wpa_supplicant/include
PRE_INCLUDES += -I$(RIOTCPU)/$(CPU)/vendor/esp-idf/wpa_supplicant/include/wpa
PRE_INCLUDES += -I$(RIOTCPU)/$(CPU)/vendor/esp-idf/wpa_supplicant/port/include
PRE_INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/include
PRE_INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/port/include
CFLAGS += -D__ets__ -DESPRESSIF_USE -DESP32_IDF_CODE=1
CFLAGS += -DEAP_PEER_METHOD -DEAP_TLS -DEAP_PEAP -DEAP_TTLS -DEAP_MSCHAPv2 -DUSE_WPA2_TASK
CFLAGS += -Wno-stringop-truncation

include $(RIOTCPU)/$(CPU)/Makefile.include

# This converts INCLUDES to a simply evaluated variable instead of a recursively
# one but only for this module.
INCLUDES := $(PRE_INCLUDES) $(INCLUDES)
