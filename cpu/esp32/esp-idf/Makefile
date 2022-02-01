MODULE=esp_idf

# Add a list of subdirectories, that should also be built:

DIRS += common

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

ifneq (,$(filter esp_idf_spi_flash,$(USEMODULE)))
  DIRS += spi_flash
endif

ifneq (,$(filter esp_idf_spi_ram,$(USEMODULE)))
  DIRS += spi_ram
endif

ifneq (,$(filter esp_idf_wifi,$(USEMODULE)))
  DIRS += wifi
endif

ifneq (,$(filter esp_idf_wpa_supplicant,$(USEMODULE)))
  DIRS += wpa_supplicant
endif

include $(RIOTBASE)/Makefile.base
