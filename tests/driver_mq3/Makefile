APPLICATION = driver_mq3
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_adc

# Define default pin mappings for some boards:
ifneq (,$(filter stm32f4discovery,$(BOARD)))
  export MQ3_ADC ?= ADC_1
  export MQ3_CHAN ?= 0
endif

USEMODULE += mq3
USEMODULE += xtimer

ifneq (,$(MQ3_ADC))
  CFLAGS += -DMQ3_ADC=$(MQ3_ADC)
else
  # set random default
  CFLAGS += -DMQ3_ADC=ADC_0
endif
ifneq (,$(MQ3_CHAN))
  CFLAGS += -DMQ3_CHAN=$(MQ3_CHAN)
else
  # set random default
  CFLAGS += -DMQ3_CHAN=0
endif

include $(RIOTBASE)/Makefile.include
