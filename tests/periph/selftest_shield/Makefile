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

# We only need 1 thread (+ the Idle thread on some platforms) and we really
# want this app working on as many boards as possible. ESP32x SoCs use an
# extra thread for esp_timer and USB stacks (stdio via USB) use an extra thread.
ifneq ($(CPU),esp32)
  ifeq (,$(filter usbus tinyusb_device,$(USEMODULE)))
    CFLAGS += -DMAXTHREADS=2
  else
    CFLAGS += -DMAXTHREADS=3
  endif
else
  ifeq (,$(filter usbus tinyusb_device,$(USEMODULE)))
    CFLAGS += -DMAXTHREADS=3
  else
    CFLAGS += -DMAXTHREADS=4
  endif
endif

ifeq (esp32-wemos-d1-r32,$(BOARD))
  # A0 (GPIO2) is pulled down by an on-board 1 kOhm resistor of the LED
  # circuit, which reduces the output voltage of the R-2R resistor ladder
  # (10 kOhm output impedance) to about 1/11, so the ADC test can't be used.
  CFLAGS += -DENABLE_ADC_TEST=0
endif

CFLAGS += \
     '-DSTOP_ON_FAILURE=$(STOP_ON_FAILURE)' \
     '-DDETAILED_OUTPUT=$(DETAILED_OUTPUT)' \
     #
