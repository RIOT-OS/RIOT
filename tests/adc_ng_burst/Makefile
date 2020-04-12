BOARD ?= arduino-mega2560
include ../Makefile.tests_common

USEMODULE += adc_ng_default
USEMODULE += adc_ng_burst
USEMODULE += fmt
USEMODULE += xtimer
# For performance comparison
USEMODULE += periph_adc

include $(RIOTBASE)/Makefile.include
