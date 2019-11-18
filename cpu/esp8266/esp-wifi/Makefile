MODULE=esp_wifi

# we have to do it in that way to avoid that pkg/lwip is found first
CFLAGS += -I$(ESP8266_RTOS_SDK_DIR)/components/lwip/lwip/src/include
CFLAGS += -I$(ESP8266_RTOS_SDK_DIR)/components/lwip/port/esp8266/include/

include $(RIOTBASE)/Makefile.base
