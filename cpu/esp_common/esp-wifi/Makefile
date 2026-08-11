MODULE=esp_wifi

MODULE_DEFS_NEEDED += \
  esp_now \
  esp_wifi \
  esp_wifi_ap \
  esp_wifi_enterprise \
  gnrc_netif_ethernet \
  od \
  #

ifeq (esp8266,$(CPU))
  # we have to do it in that way to avoid that pkg/lwip is found first
  CFLAGS += -I$(ESP8266_RTOS_SDK_DIR)/components/lwip/lwip/src/include
  CFLAGS += -I$(ESP8266_RTOS_SDK_DIR)/components/lwip/port/esp8266/include/
else
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_partition/include
endif

include $(RIOTBASE)/Makefile.base
