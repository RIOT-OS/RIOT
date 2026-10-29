MODULE = esp_idf_ieee802154

ESP32_SDK_SRC += components/ieee802154/driver/esp_ieee802154_ack.c
ESP32_SDK_SRC += components/ieee802154/driver/esp_ieee802154_dev.c
ESP32_SDK_SRC += components/ieee802154/driver/esp_ieee802154_event.c
ESP32_SDK_SRC += components/ieee802154/driver/esp_ieee802154_frame.c
ESP32_SDK_SRC += components/ieee802154/driver/esp_ieee802154_pib.c
ESP32_SDK_SRC += components/ieee802154/driver/esp_ieee802154_sec.c
ESP32_SDK_SRC += components/ieee802154/driver/esp_ieee802154_timer.c
ESP32_SDK_SRC += components/ieee802154/driver/esp_ieee802154_util.c
ESP32_SDK_SRC += components/ieee802154/esp_ieee802154.c
ESP32_SDK_SRC += components/soc/$(CPU_FAM)/ieee802154_periph.c

# The software coexistence is also enabled if BT and IEEE 802.15.4 are used,
# the coexistence sources are only compiled with WiFi otherwise.
ifneq (,$(filter esp_ble,$(USEMODULE)))
  ifeq (,$(filter esp_wifi_any,$(USEMODULE)))
    ESP32_SDK_SRC += components/esp_coex/$(CPU_FAM)/esp_coex_adapter.c
    ESP32_SDK_SRC += components/esp_coex/src/coexist.c
  endif
endif

INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_coex/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_phy/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_phy/$(CPU_FAM)/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/ieee802154/include
INCLUDES += -I$(ESP32_SDK_DIR)/components/ieee802154/private_include

#CFLAGS += -Wno-cast-function-type
#CFLAGS += -Wno-implicit-fallthrough

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
