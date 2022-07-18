MODULE = esp_idf_wifi

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/esp_event/event_send.c \
  components/esp_hw_support/port/$(CPU_FAM)/dport_access.c \
  components/esp_phy/src/phy_init.c \
  components/esp_wifi/$(CPU_FAM)/esp_adapter.c \
  components/esp_wifi/src/wifi_init.c \
  #

# additional include pathes required by this module
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_phy/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_phy/$(CPU_FAM)/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/tcpip_adapter/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/port/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/esp_supplicant/include

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
