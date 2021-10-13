BOARD ?= samr21-xpro
include ../Makefile.tests_common

USEMODULE += xtimer

FEATURES_REQUIRED += periph_rtt
DISABLE_MODULE += periph_init_rtt

RIOT_TERMINAL ?= socat

include $(RIOTBASE)/Makefile.include

# use highest possible RTT_FREQUENCY for boards that allow it
ifneq (,$(filter stm32 nrf5%,$(CPU)))
  RTT_FREQUENCY ?= RTT_MAX_FREQUENCY
  CFLAGS += -DRTT_FREQUENCY=$(RTT_FREQUENCY)
endif

# kinetis rtt runs at 1Hz, reduce samples to speed up the test
ifneq (,$(filter kinetis,$(CPU)))
  SAMPLES ?= 64
else
  SAMPLES ?= 1024
endif
CFLAGS += -DSAMPLES=$(SAMPLES)
