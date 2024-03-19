BOARD ?= nucleo-f767zi

# Custom per-board pin configuration (e.g. for setting PORT_IN, PIN_IN_0, ...)
# can be provided in a Makefile.$(BOARD) file:
-include Makefile.$(BOARD)

# Choose two output pins that do not conflict with stdio and are not connected
# to external devices such as sensors, network devices, etc.
#
# Beware: If other pins on the output port are configured as output GPIOs, they
#         might be written to during this test.
PORT_OUT ?= 0
PIN_OUT_0 ?= 0
PIN_OUT_1 ?= 1

include ../Makefile.bench_common

FEATURES_REQUIRED += periph_gpio_ll
FEATURES_REQUIRED += periph_gpio
FEATURES_OPTIONAL += periph_gpio_ll_irq
FEATURES_OPTIONAL += periph_gpio_ll_irq_level_triggered_high
FEATURES_OPTIONAL += periph_gpio_ll_irq_level_triggered_low

USEMODULE += ztimer_usec

include $(RIOTBASE)/Makefile.include

# Configure if compensation of loop overhead in the estimation of the
# toggling speed should be performed. Default: Do so, except for Cortex-M7.
# For the Cortex-M7 the loop instructions are emitted together with the GPIO
# writes due to the dual issue feature. Hence, there is no loop overhead for
# Cortex-M7 to compensate for.
ifeq (cortex-m7,$(CPU_CORE))
  COMPENSATE_OVERHEAD ?= 0
endif

COMPENSATE_OVERHEAD ?= 1

CFLAGS += -DPORT_OUT=$(PORT_OUT)
CFLAGS += -DPIN_OUT_0=$(PIN_OUT_0)
CFLAGS += -DPIN_OUT_1=$(PIN_OUT_1)
CFLAGS += -DCOMPENSATE_OVERHEAD=$(COMPENSATE_OVERHEAD)
