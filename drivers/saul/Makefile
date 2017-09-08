SRC = saul.c saul_str.c

ifneq (,$(filter saul_gpio,$(USEMODULE)))
  SRC += gpio_saul.c
endif
ifneq (,$(filter saul_adc,$(USEMODULE)))
  SRC += adc_saul.c
endif
ifneq (,$(filter saul_pwm,$(USEMODULE)))
  SRC += pwm_saul.c
endif

include $(RIOTBASE)/Makefile.base
