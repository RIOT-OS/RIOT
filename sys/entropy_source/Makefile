ifneq (,$(filter entropy_source_zero_entropy,$(USEMODULE)))
  DIRS += zero_entropy
endif

ifneq (,$(filter entropy_source_adc_noise,$(USEMODULE)))
  DIRS += adc_noise
endif

include $(RIOTBASE)/Makefile.base
