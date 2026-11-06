MODULE=esp_wifi

ifeq (esp8266,$(CPU))
  # we have to do it in that way to avoid that pkg/lwip is found first
  CFLAGS += -I$(ESP8266_RTOS_SDK_DIR)/components/lwip/lwip/src/include
  CFLAGS += -I$(ESP8266_RTOS_SDK_DIR)/components/lwip/port/esp8266/include/
else
  INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_partition/include
endif

ifneq (,$(filter esp_now,$(USEMODULE)))
  # esp_now_netdev.h is required for the declaration of esp_now_set_channel
  INCLUDES += -I$(RIOTCPU)/esp_common/esp-now
endif

include $(RIOTBASE)/Makefile.base
