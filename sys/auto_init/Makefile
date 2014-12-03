ifneq (,$(filter net_if,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/sys/net/include/
endif

ifneq (,$(filter basic_mac,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/sys/net/include/
endif

include $(RIOTBASE)/Makefile.base
