MODULE = stm32_clk

SRC = stmclk_common.c

ifneq (,$(filter $(CPU_FAM),f2 f4 f7))
  SRC += stmclk_f2f4f7.c
else ifneq (,$(filter $(CPU_FAM),f0 f1 f3))
  SRC += stmclk_f0f1f3.c
else ifneq (,$(filter $(CPU_FAM),l0 l1))
  SRC += stmclk_l0l1.c
else ifneq (,$(filter $(CPU_FAM),l4 wb))
  SRC += stmclk_l4wb.c
else ifneq (,$(filter $(CPU_FAM),l5))
  SRC += stmclk_l5.c
else ifneq (,$(filter $(CPU_FAM),g0 g4))
  SRC += stmclk_gx.c
endif

include $(RIOTBASE)/Makefile.base
