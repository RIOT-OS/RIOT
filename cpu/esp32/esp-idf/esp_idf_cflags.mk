# common definitions for all ESP-IDF modules

# indicate that ESP-IDF code is compiled
CFLAGS += -DESP_IDF_CODE

# shortcuts used by ESP-IDF
CFLAGS += -Dasm=__asm
CFLAGS += -Dtypeof=__typeof__

# required for esp_wifi (components/spi_flash/esp32/spi_flash_rom_patch.c)
# required for esp_idf_heap (components/heap/heap_caps_init.c)
# required for esp_idf_wpa_supplicant
CFLAGS += -Wno-sign-compare

# required by esp_idf_heap (components/heap/heap_caps.c)
# required for esp_idf_wpa_supplicant
CFLAGS += -Wno-implicit-function-declaration

# required for esp_wifi (components/esp_event/esp_event.c)
CFLAGS += -Wno-old-style-declaration

# required for esp_wifi (components/efuse/esp32/esp_efuse_utility.c)
# required for esp_idf_heap (components/heap/multi_heap.c)
# required for esp_idf_wpa_supplicant
CFLAGS += -Wno-old-style-definition

# required for esp_idf_common (components/bootloader_support/src/bootloader_flash.c)
CFLAGS += -Wno-unused-variable

# required for esp_idf_spi_flash (components/spi_flash/partition.c)
CFLAGS += -Wno-enum-compare

# vendor code contains casts that increase alignment requirements. Let's hope
# those are false positives.
CFLAGS += -Wno-cast-align

# TODO: required to be able to compile with GCC 14.0, remove them later
CFLAGS += -Wno-enum-conversion
CFLAGS += -Wno-incompatible-pointer-types
CFLAGS += -Wno-unused-function

# additional CFLAGS required for RISC-V architecture
ifneq (,$(filter riscv32%,$(TARGET_ARCH)))
  INCLUDES += -I$(ESP32_SDK_DIR)/components/riscv/include
  CFLAGS += -DCONFIG_IDF_TARGET_ARCH_RISCV
  CFLAGS += -march=rv32imc
  CFLAGS += -Wno-error=format=
  CFLAGS += -nostartfiles
  CFLAGS += -Wno-format
  GCC_NEW_RISCV_ISA ?= $(shell echo "typedef int dont_be_pedantic;" | \
                               $(TARGET_ARCH)-gcc -march=rv32imac -mabi=ilp32 \
                                                  -misa-spec=2.2 -E - > /dev/null 2>&1 && \
                               echo 1 || echo 0)
  ifeq (1,$(GCC_NEW_RISCV_ISA))
    CFLAGS += -misa-spec=2.2
  endif
endif
