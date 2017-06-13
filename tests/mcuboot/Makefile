APPLICATION = hello_mcuboot

default: mcuboot

include ../Makefile.tests_common

BOARD = frdm-k64f

BOARD_WHITELIST := frdm-k64f

export MCUBOOT = 1
ifdef MCUBOOT
CFLAGS += -DMCUBOOT=$(MCUBOOT)
export MCUBOOT_VERSION = 1.1.1+1
export MCUBOOT_ALIGN = 8
export MCUBOOT_HDR = 512
endif

include $(RIOTBASE)/Makefile.include
