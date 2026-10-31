BOARD ?= adafruit-feather-nrf52840-sense

RIOTBASE ?= $(CURDIR)/../../../..

USEMODULE += nrf_sf_radio

FEATURES_REQUIRED += cpu_nrf52

CFLAGS += -DNRF_SF_RADIO_ACCESS_ADDRESS=0x8367BED6U

include $(RIOTBASE)/tests/Makefile.tests_common
include $(RIOTBASE)/Makefile.include
