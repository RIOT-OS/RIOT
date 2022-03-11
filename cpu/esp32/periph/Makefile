MODULE = periph

ifneq (,$(filter periph_adc periph_dac,$(USEMODULE)))
  SRC += adc_arch.c
endif

ifneq (,$(filter periph_gpio,$(USEMODULE)))
  SRC += gpio_arch.c
endif

include $(RIOTMAKE)/periph.mk
