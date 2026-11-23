MODULE = esp_idf_eth

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/esp_eth/src/esp_eth.c \
  components/esp_eth/src/esp_eth_netif_glue.c  \
  components/esp_eth/src/mac/esp_eth_mac_esp.c \
  components/esp_eth/src/mac/esp_eth_mac_esp_dma.c \
  components/esp_eth/src/mac/esp_eth_mac_esp_gpio.c \
  components/esp_eth/src/phy/esp_eth_phy_802_3.c \
  components/esp_eth/src/phy/esp_eth_phy_generic.c  \
  components/esp_hw_support/esp_clock_output.c \
  components/esp_hal_emac/emac_hal.c \
  components/esp_hal_emac/$(CPU_FAM)/emac_clk.c \
  components/esp_hal_emac/$(CPU_FAM)/emac_periph.c \
  #

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
