BOARD ?= nucleo-f767zi

# Custom per-board pin configuration (e.g. for setting PORT_IN, PIN_IN_0, ...)
# can be provided in a Makefile.$(BOARD) file:
-include Makefile.$(BOARD)

# Choose two input pins and two pins that do not conflict with stdio. All four
# *can* be on the same GPIO port, but the two output pins and the two inputs
# pins *must* be on the same port, respectively. Connect the first input to the
# first output pin and the second input pin to the second output pin, e.g. using
# jumper wires.
PORT_IN ?=
PORT_OUT ?=
PIN_IN_0 ?= 0
PIN_IN_1 ?= 1
PIN_OUT_0 ?= 2
PIN_OUT_1 ?= 3

# Boards that require tweaking for low ROM
LOW_ROM_BOARDS := \
	nucleo-l011k4 \
	stm32f030f4-demo \
	#

ifneq (,$(filter $(BOARD),$(LOW_ROM_BOARDS)))
  LOW_ROM ?= 1
endif
LOW_ROM ?= 0

include ../Makefile.periph_common

FEATURES_REQUIRED += periph_gpio_ll
FEATURES_OPTIONAL += periph_gpio_ll_irq
FEATURES_OPTIONAL += periph_gpio_ll_irq_level_triggered_high
FEATURES_OPTIONAL += periph_gpio_ll_irq_level_triggered_low

USEMODULE += ztimer_usec

include $(RIOTBASE)/Makefile.include

ifneq (,$(PORT_OUT))
  CFLAGS += -DPORT_OUT=GPIO_PORT_$(PORT_OUT)
  CFLAGS += -DPORT_OUT_NUM=$(PORT_OUT)
endif
ifneq (,$(PORT_IN))
  CFLAGS += -DPORT_IN=GPIO_PORT_$(PORT_IN)
  CFLAGS += -DPORT_IN_NUM=$(PORT_IN)
endif
CFLAGS += -DPIN_IN_0=$(PIN_IN_0)
CFLAGS += -DPIN_IN_1=$(PIN_IN_1)
CFLAGS += -DPIN_OUT_0=$(PIN_OUT_0)
CFLAGS += -DPIN_OUT_1=$(PIN_OUT_1)
CFLAGS += -DLOW_ROM=$(LOW_ROM)

ifneq ($(CPU),esp32)
  # We only need 1 thread (+ the Idle thread on some platforms) and we really
  # want this app working on all boards.
  CFLAGS += -DMAXTHREADS=2
else
  # ESP32x SoCs uses an extra thread for esp_timer
  CFLAGS += -DMAXTHREADS=3
endif
