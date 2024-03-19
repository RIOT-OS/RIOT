include $(RIOTCPU)/efm32/efm32-info.mk

# Select the correct implementation for `periph_adc`
ifneq (,$(filter periph_adc,$(USEMODULE)))
  ifeq (2,$(EFM32_SERIES))
    SRC += adc_series2.c
  else
    SRC += adc_series01.c
  endif
endif

# Select the correct implementation for `periph_timer`
ifneq (,$(filter periph_hwrng,$(USEMODULE)))
  ifeq (1,$(EFM32_SERIES))
    SRC += hwrng_series1.c
  else ifeq (2,$(EFM32_SERIES))
    SRC += hwrng_series2.c
  endif
endif

# Select the correct implementation for `periph_rtc`
ifneq (,$(filter periph_rtc,$(USEMODULE)))
  ifeq (0,$(EFM32_SERIES))
    SRC += rtc_series0.c
  else ifeq (1,$(EFM32_SERIES))
    SRC += rtc_series1.c
  endif
endif

# Select the correct implementation for `periph_rtt`
ifneq (,$(filter periph_rtt,$(USEMODULE)))
  ifeq (0,$(EFM32_SERIES))
    SRC += rtt_series0.c
  else ifeq (1,$(EFM32_SERIES))
    SRC += rtt_series1.c
  endif
endif

# Select the correct implementation for `periph_timer`
ifneq (,$(filter periph_timer,$(USEMODULE)))
  ifeq (2,$(EFM32_SERIES))
    SRC += timer_series2.c
  else ifneq (,$(filter $(EFM32_SERIES),0 1))
    SRC += timer_series01.c
  endif
endif

# Select the correct implementation for `periph_uart`
ifneq (,$(filter periph_uart,$(USEMODULE)))
  ifeq (2,$(EFM32_SERIES))
    SRC += uart_series2.c
  else ifneq (,$(filter $(EFM32_SERIES),0 1))
    SRC += uart_series01.c
  endif
endif

# Select the correct implementation for `periph_wdt`
ifneq (,$(filter periph_wdt,$(USEMODULE)))
  ifeq (0,$(EFM32_SERIES))
    SRC += wdt_series0.c
  else ifneq (,$(filter $(EFM32_SERIES),1 2))
    SRC += wdt_series12.c
  endif
endif

include $(RIOTMAKE)/periph.mk
