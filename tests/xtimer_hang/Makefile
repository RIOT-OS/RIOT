include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno nucleo-f031k6

USEMODULE += xtimer

TEST_ON_CI_WHITELIST += all

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
#CFLAGS += -DWORKER_THREAD_PIN=GPIO_PIN\(5,7\)
#CFLAGS += -DMAIN_THREAD_PIN=GPIO_PIN\(5,6\)

include $(RIOTBASE)/Makefile.include
