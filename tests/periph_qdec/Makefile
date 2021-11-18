BOARD ?= nucleo-f401re
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_qdec

USEMODULE += xtimer

# include boards that have are modified to add a qdec peripheral
EXTERNAL_BOARD_DIRS = $(CURDIR)/boards_modded

include $(RIOTBASE)/Makefile.include
