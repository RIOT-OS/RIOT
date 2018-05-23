BOARD ?= b-l072z-lrwan1

include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6 nucleo-f042k6 nucleo-l031k6

BOARD_BLACKLIST := msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

LORA_DRIVER ?= sx1276
LORA_REGION ?= EU868

USEPKG += semtech-loramac
USEMODULE += $(LORA_DRIVER)

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += fmt

CFLAGS += -DREGION_$(LORA_REGION)

include $(RIOTBASE)/Makefile.include
