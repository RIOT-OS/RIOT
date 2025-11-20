BOARD ?= arduino-due

include ../Makefile.periph_common

FEATURES_REQUIRED += arduino_pins
FEATURES_REQUIRED += arduino_shield_uno

FEATURES_OPTIONAL += arduino_analog
FEATURES_OPTIONAL += arduino_i2c
FEATURES_OPTIONAL += arduino_pwm
FEATURES_OPTIONAL += arduino_shield_isp
FEATURES_OPTIONAL += arduino_spi
FEATURES_OPTIONAL += arduino_uart
FEATURES_OPTIONAL += periph_adc
FEATURES_OPTIONAL += periph_gpio
FEATURES_OPTIONAL += periph_gpio_irq
FEATURES_OPTIONAL += periph_i2c
FEATURES_OPTIONAL += periph_pwm
FEATURES_OPTIONAL += periph_spi
FEATURES_OPTIONAL += periph_timer
FEATURES_OPTIONAL += periph_timer_query_freqs
FEATURES_OPTIONAL += periph_uart

USEMODULE += tiny_strerror

STOP_ON_FAILURE ?= 0
DETAILED_OUTPUT ?= 0

include $(RIOTBASE)/Makefile.include

ifneq ($(CPU),esp32)
  # We only need 1 thread (+ the Idle thread on some platforms) and we really
  # want this app working on as many boards as possible
  CFLAGS += -DMAXTHREADS=2
else
  # ESP32x SoCs uses an extra thread for esp_timer
  CFLAGS += -DMAXTHREADS=3
endif

CFLAGS += \
     '-DSTOP_ON_FAILURE=$(STOP_ON_FAILURE)' \
     '-DDETAILED_OUTPUT=$(DETAILED_OUTPUT)' \
     #
