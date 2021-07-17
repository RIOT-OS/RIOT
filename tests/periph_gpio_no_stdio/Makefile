include ../Makefile.tests_common

# Select pins suitable for testing
# ================================
#
# For each input mode one pin is configured accordingly and an output pin is used to display the
# current value read. It is suggested that the corresponding output pins are connected to LEDs, so
# that the output level is visualized.
#
# For each GPIO IRQ flank one pin is configured accordingly and an output pin is toggled on each
# IRQ triggered. Note that no debouncing is done in software, so expect some flickering on the
# output pin if you don't connect a Schmitt trigger or so to the inputs.

# Pin to use as floating input and corresponding output pin
PIN_IN          := GPIO_PIN(0, 0)
PIN_OUT_1       := GPIO_PIN(0, 6)

# Pin to use as input with pull up resistor and corresponding output pin
PIN_IN_PU       := GPIO_PIN(0, 1)
PIN_OUT_2       := GPIO_PIN(0, 7)

# Pin to use as input with pull down resistor and corresponding output pin
PIN_IN_PD       := GPIO_PIN(0, 2)
PIN_OUT_3       := GPIO_PIN(0, 8)

# Pin to configure as input and configure an IRQ on rising edge and corresponding output pin
PIN_IRQ_RISING  := GPIO_PIN(0, 3)
PIN_OUT_4       := GPIO_PIN(0, 9)

# Pin to configure as input and configure an IRQ on falling edge and corresponding output pin
PIN_IRQ_FALLING := GPIO_PIN(0, 4)
PIN_OUT_5       := GPIO_PIN(0, 10)

# Pin to configure as input and configure an IRQ on both edges and corresponding output pin
PIN_IRQ_BOTH    := GPIO_PIN(0, 5)
PIN_OUT_6       := GPIO_PIN(0, 11)

# Input mode to use of IRQ pins. Default is input with pull up enabled, as this allows using
# a jumper wire connected to GND to trigger the IRQs easily
PIN_IRQ_MODE    := GPIO_IN_PU

FEATURES_REQUIRED += periph_gpio
FEATURES_OPTIONAL += periph_gpio_irq

# drop stdio, we're not using it anyway
DEFAULT_MODULE += stdio_null

include $(RIOTBASE)/Makefile.include

CFLAGS += '-DPIN_IN=$(PIN_IN)' '-DPIN_OUT_1=$(PIN_OUT_1)'
CFLAGS += '-DPIN_IN_PU=$(PIN_IN_PU)' '-DPIN_OUT_2=$(PIN_OUT_2)'
CFLAGS += '-DPIN_IN_PD=$(PIN_IN_PD)' '-DPIN_OUT_3=$(PIN_OUT_3)'
CFLAGS += '-DPIN_IRQ_RISING=$(PIN_IRQ_RISING)' '-DPIN_OUT_4=$(PIN_OUT_4)'
CFLAGS += '-DPIN_IRQ_FALLING=$(PIN_IRQ_FALLING)' '-DPIN_OUT_5=$(PIN_OUT_5)'
CFLAGS += '-DPIN_IRQ_BOTH=$(PIN_IRQ_BOTH)' '-DPIN_OUT_6=$(PIN_OUT_6)'
CFLAGS += '-DPIN_IRQ_MODE=$(PIN_IRQ_MODE)'
ifeq (1,$(NO_PULL_UP))
  $(warning PLEASE NOTE: The test for input with internal pull up has been disabled)
  CFLAGS += -DNO_PULL_UP=1
endif
ifeq (1,$(NO_PULL_DOWN))
  $(warning PLEASE NOTE: The test for input with internal pull down has been disabled)
  CFLAGS += -DNO_PULL_DOWN=1
endif
