BOARD ?= samr21-xpro
include ../Makefile.tests_common

USEMODULE += xtimer

FEATURES_REQUIRED += periph_rtt
DISABLE_MODULE += periph_init_rtt

SAMPLES ?= 1024
CFLAGS += -DSAMPLES=$(SAMPLES)

include $(RIOTBASE)/Makefile.include

$(call target-export-variables, test, SAMPLES)

# use highest possible RTT_FREQUENCY for boards that allow it
ifneq (,$(filter stm32 nrf5%,$(CPU)))
  RTT_FREQUENCY ?= RTT_MAX_FREQUENCY
  CFLAGS += -DRTT_FREQUENCY=$(RTT_FREQUENCY)
endif
