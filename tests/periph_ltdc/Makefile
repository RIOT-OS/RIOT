BOARD ?= stm32f746g-disco
include ../Makefile.tests_common

FEATURES_REQUIRED += periph_ltdc

USEMODULE += ztimer
USEMODULE += ztimer_msec

# add include path for riot_logo.h
INCLUDES += -I$(RIOTBASE)/tests

include $(RIOTBASE)/Makefile.include
