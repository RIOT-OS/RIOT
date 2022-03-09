MODULE=esp_idf_wpa_supplicant_port

include $(RIOTBASE)/Makefile.base

# we have to do it in that way to avoid that $(RIOTBASE)/sys/include/crypto
# is found first
PRE_INCLUDES += -I$(RIOTCPU)/$(CPU)/vendor/esp-idf/wpa_supplicant/port/include
PRE_INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/include
PRE_INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/port/include
CFLAGS += -D__ets__

include $(RIOTCPU)/$(CPU)/Makefile.include

# This converts INCLUDES to a simply evaluated variable instead of a recursively
# one but only for this module.
INCLUDES := $(PRE_INCLUDES) $(INCLUDES)
