MODULE=esp_idf

export ESP_IDF_PATH = $(shell pwd)

# Add a list of subdirectories, that should also be built:

ifneq (,$(filter esp_idf_adc,$(USEMODULE)))
  DIRS += adc
endif

ifneq (,$(filter esp_idf_ble,$(USEMODULE)))
  DIRS += ble
endif

ifneq (,$(filter esp_idf_common,$(USEMODULE)))
  DIRS += common
endif

ifneq (,$(filter esp_idf_efuse,$(USEMODULE)))
  DIRS += efuse
endif

ifneq (,$(filter esp_idf_eth,$(USEMODULE)))
  DIRS += eth
endif

ifneq (,$(filter esp_idf_event,$(USEMODULE)))
  DIRS += event
endif

ifneq (,$(filter esp_idf_gpio,$(USEMODULE)))
  DIRS += gpio
endif

ifneq (,$(filter esp_idf_heap,$(USEMODULE)))
  DIRS += heap
endif

ifneq (,$(filter esp_idf_nvs_flash,$(USEMODULE)))
  DIRS += nvs_flash
endif

ifneq (,$(filter esp_idf_rmt,$(USEMODULE)))
  DIRS += rmt
endif

ifneq (,$(filter esp_idf_spi_flash,$(USEMODULE)))
  DIRS += spi_flash
endif

ifneq (,$(filter esp_idf_spi_ram,$(USEMODULE)))
  DIRS += spi_ram
endif

ifneq (,$(filter esp_idf_usb,$(USEMODULE)))
  DIRS += usb
endif

ifneq (,$(filter esp_idf_wifi,$(USEMODULE)))
  DIRS += wifi
endif

ifneq (,$(filter esp_idf_wpa_supplicant,$(USEMODULE)))
  DIRS += wpa_supplicant
endif

include $(RIOTBASE)/Makefile.base
