# common definitions for all ESP-IDF modules

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

# required for esp-wifi (components/efuse/esp32/esp_efuse_utility.c)
# required for esp_idf_heap (components/heap/multi_heap.c)
# required for esp_idf_wpa_supplicant
CFLAGS += -Wno-old-style-definition

# vendor code contains casts that increase alignment requirements. Let's hope
# those are false positives.
CFLAGS += -Wno-cast-align
