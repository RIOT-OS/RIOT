BOARD ?= nrf52dk

include ../Makefile.tests_common

export IMAGE_VERSION = 1.1.1+1

# this test is supposed to always build the mcuboot image
all: mcuboot

BOARD_WHITELIST := \
  nrf52dk \
  #

include $(RIOTBASE)/Makefile.include
