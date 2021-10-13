MODULE=esp_idf_nvs_flash

include $(RIOTBASE)/Makefile.base

CFLAGS   += -DESP_PLATFORM
CXXFLAGS += -std=c++11
INCLUDES += -I$(RIOTCPU)/$(CPU)/vendor/esp-idf/include/log
INCLUDES += -I$(ESP32_SDK_DIR)/components/nvs_flash/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/spi_flash/include
