include ../Makefile.drivers_common

EXTERNAL_BOARD_DIRS = $(CURDIR)/external_boards


BOARD ?= nrf52840dk_qdec


USEMODULE += inc_encoder
#USEMODULE += inc_encoder_software
USEMODULE += inc_encoder_hardware
USEMODULE += ztimer_sec

include $(RIOTBASE)/Makefile.include
