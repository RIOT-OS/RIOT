APPLICATION = driver_mq3
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_adc

USEMODULE += mq3
USEMODULE += xtimer

# set default device parameters in case they are undefined
MQ3_ADC  ?= ADC_0
MQ3_CHAN ?= 0

# export parameters
CFLAGS += -DMQ3_ADC=$(MQ3_ADC)
CFLAGS += -DMQ3_CHAN=$(MQ3_CHAN)

include $(RIOTBASE)/Makefile.include
