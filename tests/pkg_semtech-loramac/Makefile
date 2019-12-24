BOARD ?= b-l072z-lrwan1

include ../Makefile.tests_common

# waspmote-pro and arduino-meag2560 don't have enough RAM to support another
# thread dedicated to RX messages
BOARD_WITHOUT_LORAMAC_RX := arduino-mega2560 waspmote-pro

LORA_DRIVER ?= sx1276
LORA_REGION ?= EU868

USEPKG += semtech-loramac
USEMODULE += $(LORA_DRIVER)

# load loramac RX if board supports it
ifeq (,$(filter $(BOARD),$(BOARD_WITHOUT_LORAMAC_RX)))
  USEMODULE += semtech_loramac_rx
endif

USEMODULE += auto_init_loramac
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += fmt

FEATURES_OPTIONAL += periph_eeprom

include $(RIOTBASE)/Makefile.include
