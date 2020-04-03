MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS += periph
DIRS += $(RIOTCPU)/cortexm_common

ifneq (,$(filter cpu_efm32gg,$(USEMODULE)))
  DIRS += families/efm32gg
endif
ifneq (,$(filter cpu_efm32lg,$(USEMODULE)))
  DIRS += families/efm32lg
endif
ifneq (,$(filter cpu_efm32pg1b,$(USEMODULE)))
  DIRS += families/efm32pg1b
endif
ifneq (,$(filter cpu_efm32pg12b,$(USEMODULE)))
  DIRS += families/efm32pg12b
endif
ifneq (,$(filter cpu_efr32mg1p,$(USEMODULE)))
  DIRS += families/efr32mg1p
endif
ifneq (,$(filter cpu_efr32mg12p,$(USEMODULE)))
  DIRS += families/efr32mg12p
endif

include $(RIOTBASE)/Makefile.base
