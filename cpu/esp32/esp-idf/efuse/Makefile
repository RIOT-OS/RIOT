MODULE = esp_idf_efuse

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/efuse/$(CPU_FAM)/esp_efuse_fields.c \
  components/efuse/$(CPU_FAM)/esp_efuse_table.c \
  components/efuse/$(CPU_FAM)/esp_efuse_utility.c \
  components/efuse/src/esp_efuse_api.c \
  components/efuse/src/esp_efuse_utility.c \
  components/esp_hw_support/mac_addr.c \
  #

ifneq (,$(filter esp32,$(CPU_FAM)))
  ESP32_SDK_SRC += components/efuse/src/efuse_controller/keys/without_key_purposes/three_key_blocks/esp_efuse_api_key.c
else
  ESP32_SDK_SRC += components/efuse/src/efuse_controller/keys/with_key_purposes/esp_efuse_api_key.c
endif

# additional include pathes required by this module
INCLUDES += -I$(ESP32_SDK_DIR)/components/efuse/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/efuse/private_include
INCLUDES += -I$(ESP32_SDK_DIR)/components/efuse/$(CPU_FAM)/private_include

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
