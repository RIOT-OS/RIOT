MODULE = esp_idf_event

# source files to be compiled for this module
ESP32_SDK_SRC = \
  components/esp_event/default_event_loop.c \
  components/esp_event/esp_event.c \
  #

# additional include pathes required by this module
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_event/private_include
INCLUDES += -I$(ESP32_SDK_DIR)/components/esp_event/include

include $(RIOTBASE)/Makefile.base

ESP32_SDK_BIN = $(BINDIR)/$(MODULE)

include ../esp_idf.mk
include ../esp_idf_cflags.mk
