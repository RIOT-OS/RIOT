MODULE=esp_idf_wpa_supplicant_port

include $(RIOTBASE)/Makefile.base

# we have to do it in that way to avoid that $(RIOTBASE)/sys/include/crypto
# is found first
INCLUDES  = -I$(CPUDIR)/vendor/esp-idf/wpa_supplicant/port/include
INCLUDES += -I$(ESP8266_RTOS_SDK_DIR)/components/wpa_supplicant/include
INCLUDES += -I$(ESP8266_RTOS_SDK_DIR)/components/wpa_supplicant/port/include
CFLAGS += -D__ets__ -DESPRESSIF_USE -DESP_PLATFORM=1
CFLAGS += -Wno-strict-aliasing

include $(CPUDIR)/Makefile.include

INCLUDES += -I$(RIOTBASE)/core/include
INCLUDES += -I$(RIOTBASE)/sys/include
INCLUDES += -I$(CPUDIR)/vendor/esp-idf/log/include
INCLUDES += -I$(CPUDIR)/include
INCLUDES += -I$(BOARDDIR)/include
