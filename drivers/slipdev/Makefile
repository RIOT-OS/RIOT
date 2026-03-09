SRC := slipdev.c net.c

ifneq (,$(filter slipdev_stdio,$(USEMODULE)))
	SRC += stdio.c
endif

ifneq (,$(filter slipdev_config,$(USEMODULE)))
	SRC += coap.c
endif

include $(RIOTBASE)/Makefile.base
