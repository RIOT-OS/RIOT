BOARD ?= arduino-mega2560
include ../Makefile.tests_common

USEMODULE += adc_ng_default
USEMODULE += adc_ng_util
USEMODULE += fmt
USEMODULE += shell

# This tests also exposes access to the periph_adc API, if used. You can either
# use the native periph_adc API (to compare outputs of the two drivers) or
# test the periph_adc wrapper, but *NOT* *BOTH* at the same time!
USEMODULE += adc_ng_compat          # <- Use wrapper on top of ADC NG
#FEATURES_OPTIONAL += periph_adc    # <- Use native periph_adc API

include $(RIOTBASE)/Makefile.include
