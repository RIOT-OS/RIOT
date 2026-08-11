# Define the module that is built:
MODULE = cpu

MODULE_DEFS_NEEDED += \
  esp_gdbstub \
  esp_idf_heap \
  esp_log_startup \
  esp_wifi_any \
  mtd \
  #

# Add a list of subdirectories, that should also be built:
DIRS += $(RIOTCPU)/esp_common
DIRS += freertos
DIRS += periph
DIRS += sdk
DIRS += vendor

INCLUDES += -I$(ESP8266_RTOS_SDK_DIR)/components/esp8266/include/internal

include $(RIOTBASE)/Makefile.base
