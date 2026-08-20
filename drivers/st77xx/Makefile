
MODULE_DEFS_NEEDED += \
  st7735 \
  st7789 \
  st7796 \
  #
SRC = st77xx.c

ifneq (,$(filter st7735,$(USEMODULE)))
  SRC += st7735_init.c
endif

ifneq (,$(filter st7789,$(USEMODULE)))
  SRC += st7789_init.c
endif

ifneq (,$(filter st7796,$(USEMODULE)))
  SRC += st7796_init.c
endif

include $(RIOTBASE)/Makefile.base
