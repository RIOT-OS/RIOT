MODULE = periph

ifneq (,$(filter periph_adc periph_dac,$(USEMODULE)))
  SRC += adc_arch_$(CPU_FAM).c
endif

ifneq (,$(filter periph_gpio,$(USEMODULE)))
  SRC += gpio_arch_$(CPU_FAM).c
endif

include $(RIOTMAKE)/periph.mk
