BOARD ?= samr21-xpro
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno

FEATURES_REQUIRED = periph_i2c

USEMODULE += shell

CFLAGS += -DRIOT_APPLICATION=\"$(APPLICATION)\"

include $(RIOTBASE)/Makefile.include
