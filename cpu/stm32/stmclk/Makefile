MODULE = stm32_clk

SRC = stmclk_common.c

_CPU_FAM_SHORT = $(subst stm32,,$(CPU_FAM))
ifneq (,$(filter f%,$(_CPU_FAM_SHORT)))
  SRC += stmclk_fx.c
else ifneq (,$(filter $(_CPU_FAM_SHORT),l0 l1))
  SRC += stmclk_l0l1.c
else ifneq (,$(filter $(_CPU_FAM_SHORT),l4 wb))
  SRC += stmclk_l4wb.c
endif

include $(RIOTBASE)/Makefile.base
