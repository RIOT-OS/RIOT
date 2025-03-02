MODULE = esp_idf_ble

ifeq (esp32,$(CPU_FAM))
  # source files to be compiled for this module
  ESP32_SDK_SRC += components/bt/controller/$(CPU_FAM)/bt.c
  ESP32_SDK_SRC += components/esp_system/esp_system.c
else ifneq (,$(filter esp32s3 esp32c3,$(CPU_FAM)))
  # source files to be compiled for this module
  ESP32_SDK_SRC += components/bt/controller/esp32c3/bt.c
  ESP32_SDK_SRC += components/esp_system/esp_system.c
endif

ifeq (,$(filter esp_idf_wifi,$(USEMODULE)))
  ESP32_SDK_SRC += \
    components/esp_phy/$(CPU_FAM)/phy_init_data.c \
    components/esp_phy/src/phy_common.c \
    components/esp_phy/src/phy_init.c \
    components/esp_system/port/soc/$(CPU_FAM)/reset_reason.c \
    components/esp_wifi/src/wifi_init.c \
    components/soc/esp32/dport_access.c \
    #
endif

INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_coex/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_event/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_netif/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_partition/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_phy/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_phy/$(CPU_FAM)/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_wifi/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/nvs_flash/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/esp_supplicant/include

CFLAGS += -Wno-cast-function-type
CFLAGS += -Wno-implicit-fallthrough

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
