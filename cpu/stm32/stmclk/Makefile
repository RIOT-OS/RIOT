MODULE = stm32_clk

SRC = stmclk_common.c

ifneq (,$(filter f%,$(CPU_FAM)))
  SRC += stmclk_fx.c
else ifneq (,$(filter $(CPU_FAM),l0 l1))
  SRC += stmclk_l0l1.c
else ifneq (,$(filter $(CPU_FAM),l4 wb))
  SRC += stmclk_l4wb.c
else ifneq (,$(filter $(CPU_FAM),g0 g4))
  SRC += stmclk_gx.c
endif

include $(RIOTBASE)/Makefile.base
