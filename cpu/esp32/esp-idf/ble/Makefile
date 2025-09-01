MODULE = esp_idf_ble

ifeq (esp32,$(CPU_FAM))
  # source files to be compiled for this module for ESP32
  ESP32_SDK_SRC += components/bt/controller/$(CPU_FAM)/bt.c
  ESP32_SDK_SRC += components/esp_system/esp_system.c
else ifneq (,$(filter esp32c6 esp32h2,$(CPU_FAM)))
  # source files to be compiled for this module for ESP32H2
  ESP32_SDK_SRC += components/bt/controller/$(CPU_FAM)/bt.c
  ESP32_SDK_SRC += components/bt/porting/mem/bt_osi_mem.c
  ESP32_SDK_SRC += components/bt/porting/mem/os_msys_init.c
  ESP32_SDK_SRC += components/bt/porting/npl/freertos/src/npl_os_freertos.c
  ESP32_SDK_SRC += components/bt/porting/transport/driver/vhci/hci_driver_standard.c
  ESP32_SDK_SRC += components/bt/porting/transport/src/hci_transport.c
else ifneq (,$(filter esp32s3 esp32c3,$(CPU_FAM)))
  # source files to be compiled for this module for ESP32C3 and ESP32S3
  ESP32_SDK_SRC += components/bt/controller/esp32c3/bt.c
  ESP32_SDK_SRC += components/esp_system/esp_system.c
endif

ifneq (,$(filter esp32c6 esp32h2,$(CPU_FAM)))
  # ESP-IDF npl includes have to be found before the includes of pkg/nimble
  INCLUDES := -I$(ESP32_SDK_DIR)/components/bt/porting/npl/freertos/include $(INCLUDES)
  INCLUDES += -I$(ESP32_SDK_DIR)/components/bt/porting/transport/include
  INCLUDES += -I$(ESP32_SDK_DIR)/components/bt/porting/include
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
