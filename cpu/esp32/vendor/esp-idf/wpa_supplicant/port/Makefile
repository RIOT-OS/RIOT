MODULE=esp_idf_wpa_supplicant_port

include $(RIOTBASE)/Makefile.base

# we have to do it in that way to avoid that $(RIOTBASE)/sys/include/crypto
# is found first
INCLUDES  = -I$(RIOTCPU)/$(CPU)/vendor/esp-idf/wpa_supplicant/port/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/port/include
CFLAGS += -D__ets__

include $(RIOTCPU)/$(CPU)/Makefile.include

INCLUDES += -I$(RIOTBASE)/core/include
INCLUDES += -I$(RIOTBASE)/drivers/include
INCLUDES += -I$(RIOTCPU)/$(CPU)/include
