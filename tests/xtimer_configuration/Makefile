include ../Makefile.tests_common

USEMODULE += xtimer

# Port and pin configuration for probing with oscilloscope
# Define Test pin for hardware timer interrupt, hardware dependent
# For all ATmega Platforms
CFLAGS += -DDEBUG_TIMER_PORT=PORTF
CFLAGS += -DDEBUG_TIMER_DDR=DDRF
CFLAGS += -DDEBUG_TIMER_PIN=PORTF4

# Define test probing pins GPIO API based.
# Port number should be found in port enum e.g in cpu/include/periph_cpu.h
FEATURES_REQUIRED += periph_gpio
# Jiminy probing Pins
CFLAGS += -DSLEEP_PIN_1=GPIO_PIN\(5,5\)
CFLAGS += -DSLEEP_PIN_2=GPIO_PIN\(5,6\)
CFLAGS += -DSLEEP_PIN_3=GPIO_PIN\(5,7\)

include $(RIOTBASE)/Makefile.include

test:
	./tests/01-run.py
