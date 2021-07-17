BOARD ?= esp32-wroom-32

include ../Makefile.tests_common

FEATURES_REQUIRED += arch_esp32
FEATURES_REQUIRED += periph_i2c

USEMODULE += esp_i2c_hw
USEMODULE += shell

include $(RIOTBASE)/Makefile.include
