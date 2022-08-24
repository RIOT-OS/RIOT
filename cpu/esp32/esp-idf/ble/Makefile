MODULE = esp_idf_ble

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/bt/controller/$(CPU_FAM)/bt.c \
  #

ifeq (,$(filter esp_idf_wifi,$(USEMODULE)))
  ESP32_SDK_SRC += \
    components/esp_hw_support/port/$(CPU_FAM)/dport_access.c \
    components/esp_phy/src/phy_init.c \
    components/esp_wifi/src/wifi_init.c \
    #
endif

INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_event/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_netif/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_phy/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_phy/$(CPU_FAM)/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_wifi/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/nvs_flash/include

INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/esp_supplicant/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/tcpip_adapter/include

CFLAGS += -Wno-cast-function-type
CFLAGS += -Wno-implicit-fallthrough

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
