MODULE=esp_idf

DIRS += driver
DIRS += esp32
DIRS += soc
DIRS += spi_flash

ifneq (,$(filter esp_idf_heap,$(USEMODULE)))
    DIRS += heap
endif

ifneq (,$(filter esp_wifi_any,$(USEMODULE)))
    DIRS += nvs_flash
    DIRS += wpa_supplicant
    INCLUDES += -I$(ESP32_SDK_DIR)/components/smartconfig_ack/include
endif

ifneq (,$(filter esp_eth,$(USEMODULE)))
    DIRS += ethernet
endif

include $(RIOTBASE)/Makefile.base
