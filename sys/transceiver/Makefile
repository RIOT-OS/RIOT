MODULE =transceiver

ifneq (,$(findstring cc2420,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/sys/net/include
endif

ifneq (,$(findstring at86rf231,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/sys/net/include
endif

ifneq (,$(findstring mc1322x,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/sys/net/include
endif

include $(MAKEBASE)/Makefile.base
