SRC = epson_escpos.c

ifneq (,$(filter epson_escpos_tcp,$(USEMODULE)))
  SRC += epson_escpos_tcp.c
endif

ifneq (,$(filter epson_escpos_uart,$(USEMODULE)))
  SRC += epson_escpos_uart.c
endif

ifneq (,$(filter epson_escpos_dummy,$(USEMODULE)))
  SRC += epson_escpos_dummy.c
endif

include $(RIOTBASE)/Makefile.base
