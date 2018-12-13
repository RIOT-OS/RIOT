BOARD ?= nrf52dk

include ../Makefile.tests_common

BOARD_WHITELIST := nrf52dk

export IMAGE_VERSION = 1.1.1+1

# this test is supposed to always build the mcuboot image
all: mcuboot

include $(RIOTBASE)/Makefile.include

# this is kind of ugly, but there is not IMGTOOL_FLAGS.
IMGTOOL_DETERMINISTIC ?= $(RIOT_CI_BUILD)

ifeq ($(IMGTOOL_DETERMINISTIC),1)
	IMGTOOL += --seed RIOT-is-awesome
endif
