MODULE = esp_idf_nvs_flash

# source files to be compiled for this module
ESP32_SDK_SRCXX = \
  components/nvs_flash/src/nvs_api.cpp \
  components/nvs_flash/src/nvs_cxx_api.cpp \
  components/nvs_flash/src/nvs_handle_locked.cpp \
  components/nvs_flash/src/nvs_handle_simple.cpp \
  components/nvs_flash/src/nvs_item_hash_list.cpp \
  components/nvs_flash/src/nvs_page.cpp \
  components/nvs_flash/src/nvs_pagemanager.cpp \
  components/nvs_flash/src/nvs_partition.cpp \
  components/nvs_flash/src/nvs_partition_lookup.cpp \
  components/nvs_flash/src/nvs_partition_manager.cpp \
  components/nvs_flash/src/nvs_storage.cpp \
  components/nvs_flash/src/nvs_types.cpp \
  #

# additional include pathes required by this module
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_rom/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/nvs_flash/include

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk

# vendor code contains casts that increase alignment requirements. Let's hope
# those are false positives.
CFLAGS += -Wno-cast-align

# TODO: required to be able to compile with GCC 12.1, remove them after upgrade to ESP-IDF 5.1
CFLAGS += -Wno-error=format=
CFLAGS += -Wno-format

# additional CFLAGS required for RISC-V architecture
ifneq (,$(filter riscv32%,$(TARGET_ARCH)))
  CFLAGS += -Wno-error=format=
  CFLAGS += -Wno-format
endif
