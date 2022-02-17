include $(RIOTCPU)/efm32/efm32-info.mk

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

# Select the correct implementation for `periph_wdt`
ifneq (,$(filter periph_wdt,$(USEMODULE)))
  ifeq (0,$(EFM32_SERIES))
    SRC += wdt_series0.c
  else ifeq (1,$(EFM32_SERIES))
    SRC += wdt_series1.c
  endif
endif

include $(RIOTMAKE)/periph.mk
