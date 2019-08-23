include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-leonardo arduino-nano\
                             arduino-uno
DRIVER ?= mpu9150

USEMODULE += $(DRIVER)
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
