include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno \
                             nucleo-f031k6 nucleo-f042k6

USEMODULE += xtimer

# Port and pin configuration for probing with oscilloscope
# Define Test pin for hardware timer interrupt, hardware dependent
# ATmega
#CFLAGS += -DDEBUG_TIMER_PORT=PORTF
#CFLAGS += -DDEBUG_TIMER_DDR=DDRF
#CFLAGS += -DDEBUG_TIMER_PIN=PORTF4

# Define test probing pins GPIO API based.
# Port number should be found in port enum e.g in cpu/include/periph_cpu.h
#FEATURES_REQUIRED += periph_gpio
# Jiminy probing Pins
#CFLAGS += -DMAIN_THREAD_PIN=5
#CFLAGS += -DMAIN_THREAD_PORT=PORT_F
#CFLAGS += -DSLACKER_THREAD_PIN=6
#CFLAGS += -DSLACKER_THREAD_PORT=PORT_F
#CFLAGS += -DPRINT_THREAD_PIN=7
#CFLAGS += -DPRINT_THREAD_PORT=PORT_F

include $(RIOTBASE)/Makefile.include
