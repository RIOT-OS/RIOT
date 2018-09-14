BOARD ?= nrf52dk

include ../Makefile.tests_common

BOARD_WHITELIST := nrf52dk

export IMAGE_VERSION = 1.1.1+1

# this test is supposed to always build the mcuboot image
all: mcuboot

include $(TESTBASE)/../Makefile.include
