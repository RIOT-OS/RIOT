BOARD ?= nrf52dk

include ../Makefile.tests_common

export IMAGE_VERSION = 1.1.1+1

# this test is supposed to always build the mcuboot image
ifneq ($(BUILD_IN_DOCKER),1)
# HACK: When building with docker, mcuboot target must be done only in docker
all: mcuboot
endif

BOARD_WHITELIST := \
  nrf52dk \
  #

include $(RIOTBASE)/Makefile.include
