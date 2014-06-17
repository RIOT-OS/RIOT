ifneq (,$(filter cc2420,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/sys/net/include
endif

ifneq (,$(filter at86rf231,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/sys/net/include
endif

ifneq (,$(filter mc1322x,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/sys/net/include
endif

include $(RIOTBASE)/Makefile.base
