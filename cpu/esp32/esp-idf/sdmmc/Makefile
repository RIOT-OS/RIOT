MODULE = esp_idf_sdmmc

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/driver/sdmmc_host.c \
  components/driver/sdmmc_transaction.c \
  components/soc/$(CPU_FAM)/sdmmc_periph.c \
  #

# additional include pathes required by this module
# INCLUDES += -I$(ESP32_SDK_DIR)/components/driver/include

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
