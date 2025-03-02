MODULE = esp_idf_api

# ESP-IDF header files must be found first in this module. Therefore,
# the ESP-IDF include paths must come before the RIOT include paths.
PRE_INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_common/include
PRE_INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_driver_gpio/include
PRE_INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_driver_uart/include
PRE_INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_hw_support/include
PRE_INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_rom/include
PRE_INCLUDES += -I$(ESP32_SDK_DIR)/components/hal/include
PRE_INCLUDES += -I$(ESP32_SDK_DIR)/components/hal/platform_port/include
PRE_INCLUDES += -I$(ESP32_SDK_DIR)/components/soc/include

PRE_INCLUDES += -I$(ESP32_SDK_DIR)/components/hal/$(CPU_FAM)/include
PRE_INCLUDES += -I$(ESP32_SDK_DIR)/components/soc/$(CPU_FAM)/include

include $(RIOTBASE)/Makefile.base

INCLUDES := $(PRE_INCLUDES) $(INCLUDES)
