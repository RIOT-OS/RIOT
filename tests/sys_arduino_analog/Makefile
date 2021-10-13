BOARD ?= arduino-zero

include ../Makefile.tests_common

LED_PIN ?= 3

USEMODULE += arduino

# Features used by Arduino analogRead/analogWrite functions are required
FEATURES_REQUIRED += arduino_pwm
FEATURES_REQUIRED += periph_adc

CFLAGS += -DLED_PIN=$(LED_PIN)

include $(RIOTBASE)/Makefile.include
