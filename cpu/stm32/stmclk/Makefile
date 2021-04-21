MODULE = stm32_clk

ifneq (mp1,$(CPU_FAM))
  SRC = stmclk_common.c
endif

ifneq (,$(filter $(CPU_FAM),f2 f4 f7))
  SRC += stmclk_f2f4f7.c
else ifneq (,$(filter $(CPU_FAM),f0 f1 f3))
  SRC += stmclk_f0f1f3.c
else ifneq (,$(filter $(CPU_FAM),l0 l1))
  SRC += stmclk_l0l1.c
else ifneq (,$(filter $(CPU_FAM),l4 wb wl))
  SRC += stmclk_l4wx.c
else ifneq (,$(filter $(CPU_FAM),l5))
  SRC += stmclk_l5.c
else ifneq (,$(filter $(CPU_FAM),g0 g4))
  SRC += stmclk_gx.c
else ifneq (,$(filter $(CPU_FAM),mp1))
  ifneq (,$(filter stm32mp1_eng_mode,$(USEMODULE)))
    SRC = stmclk_mp1.c
  else
    # Do not use *.c as SRC if SRC is empty (see Makefile.base)
    NO_AUTO_SRC = 1
  endif
endif

include $(RIOTBASE)/Makefile.base
