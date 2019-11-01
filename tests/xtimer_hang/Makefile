include ../Makefile.tests_common

USEMODULE += xtimer
DISABLE_MODULE := core_msg

# Port and pin configuration for probing with oscilloscope
# Define Test pin for hardware timer interrupt, hardware dependent
# For all ATmega Platforms
#CFLAGS += -DDEBUG_TIMER_PORT=PORTF
#CFLAGS += -DDEBUG_TIMER_DDR=DDRF
#CFLAGS += -DDEBUG_TIMER_PIN=PORTF4

# Define test probing pins GPIO API based.
# Port number should be found in port enum e.g in cpu/include/periph_cpu.h
#FEATURES_REQUIRED += periph_gpio
# Jiminy probing Pins
#CFLAGS += -DWORKER_THREAD_PIN_1=GPIO_PIN\(5,7\)
#CFLAGS += -DWORKER_THREAD_PIN_2=GPIO_PIN\(5,6\)
#CFLAGS += -DMAIN_THREAD_PIN=GPIO_PIN\(5,5\)

include $(RIOTBASE)/Makefile.include
