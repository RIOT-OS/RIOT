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

INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_coex/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_partition/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_wifi/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_phy/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/wpa_supplicant/esp_supplicant/include

CFLAGS += -Wno-cast-function-type
CFLAGS += -Wno-implicit-fallthrough

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
