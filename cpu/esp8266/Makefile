# Define the module that is built:
MODULE = cpu

# Add a list of subdirectories, that should also be built:
DIRS += $(RIOTCPU)/esp_common
DIRS += freertos
DIRS += periph
DIRS += sdk
DIRS += vendor

ifneq (, $(filter esp_wifi, $(USEMODULE)))
    DIRS += esp-wifi
endif

INCLUDES += -I$(ESP8266_RTOS_SDK_DIR)/components/esp8266/include/internal

include $(RIOTBASE)/Makefile.base
