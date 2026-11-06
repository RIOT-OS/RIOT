MODULE = efm32_coretemp

include $(RIOTCPU)/efm32/efm32-info.mk

SRC = coretemp_saul.c

# Select the correct implementation for the internal temperature sensor
ifeq (2,$(EFM32_SERIES))
  SRC += coretemp_series2.c
else
  SRC += coretemp_series01.c
endif

include $(RIOTBASE)/Makefile.base
