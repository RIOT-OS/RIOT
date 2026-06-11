MODULE = nrf5x_common_periph

# The nRF54L family uses dedicated GPIO and RTT (GRTC based) driver
# implementations. The UART driver is shared (see uart.c).
ifeq (nrf54l,$(CPU_FAM))
  SUBMODULES_NO_SRC += gpio.c rtt.c
  ifneq (,$(filter periph_gpio,$(USEMODULE)))
    SRC += gpio_nrf54l.c
  endif
  ifneq (,$(filter periph_rtt,$(USEMODULE)))
    SRC += rtt_grtc.c
  endif
endif

# Select the specific implementation for `periph_i2c`
ifneq (,$(filter periph_i2c,$(USEMODULE)))
  ifneq (,$(filter $(CPU_FAM),nrf52 nrf53 nrf9160))
    SRC += i2c_nrf52_nrf9160.c
  endif
  ifeq (nrf54l,$(CPU_FAM))
    SRC += i2c_nrf54l.c
  endif
endif

# Select the specific implementation for `periph_pwm`
# nRF51 has its own PWM driver variant in its periph driver folder
ifneq (,$(filter periph_pwm,$(USEMODULE)))
  ifneq (,$(filter $(CPU_FAM),nrf52 nrf53 nrf54l nrf9160))
    SRC += pwm_nrfxx.c
  endif
endif

# Select the specific implementation for `periph_spi`
ifneq (,$(filter periph_spi,$(USEMODULE)))
  ifneq (,$(filter $(CPU_FAM),nrf52 nrf53 nrf9160))
    SRC += spi_nrf52_nrf9160.c
  endif
  ifeq (nrf54l,$(CPU_FAM))
    SRC += spi_nrf54l.c
  endif
endif

include $(RIOTMAKE)/periph.mk
