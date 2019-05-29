BOARD ?= b-l072z-lrwan1

include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-leonardo arduino-nano \
                             arduino-uno nucleo-f031k6 nucleo-f042k6 \
                             nucleo-l031k6

BOARD_BLACKLIST := msb-430 msb-430h pic32-clicker pic32-wifire \
                   telosb wsn430-v1_3b wsn430-v1_4 z1

# waspmote-pro doesn't have enough RAM to support another thread dedicated to
# RX messages
BOARD_WITHOUT_LORAMAC_RX := waspmote-pro

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
