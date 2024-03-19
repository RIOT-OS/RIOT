MODULE = esp_idf_eth

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/esp_eth/src/esp_eth.c \
  components/esp_eth/src/esp_eth_mac_esp.c  \
  components/esp_eth/src/esp_eth_netif_glue.c  \
  components/esp_eth/src/esp_eth_phy.c  \
  components/esp_eth/src/esp_eth_phy_dp83848.c  \
  components/esp_eth/src/esp_eth_phy_ip101.c  \
  components/esp_eth/src/esp_eth_phy_ksz80xx.c  \
  components/esp_eth/src/esp_eth_phy_lan87xx.c  \
  components/esp_eth/src/esp_eth_phy_rtl8201.c \
  components/hal/emac_hal.c \
  #

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
