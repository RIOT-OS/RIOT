MODULE=esp_idf_wpa_supplicant_crypto

include $(RIOTBASE)/Makefile.base

# we have to do it in that way to avoid that $(RIOTBASE)/sys/include/crypto
# is found first
PRE_INCLUDES += -I$(RIOTCPU)/$(CPU)/vendor/esp-idf/wpa_supplicant/include
PRE_INCLUDES += -I$(RIOTCPU)/$(CPU)/vendor/esp-idf/wpa_supplicant/port/include
PRE_INCLUDES += -I$(ESP8266_RTOS_SDK_DIR)/components/wpa_supplicant/include
PRE_INCLUDES += -I$(ESP8266_RTOS_SDK_DIR)/components/wpa_supplicant/port/include
CFLAGS += -D__ets__ -DESPRESSIF_USE -DESP_PLATFORM=1
CFLAGS += -Wno-strict-aliasing

# This converts INCLUDES to a simply evaluated variable instead of a recursively
# one but only for this module.
INCLUDES := $(PRE_INCLUDES) $(INCLUDES)
