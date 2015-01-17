ifneq (,$(filter net_if,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/sys/net/include/
endif

ifneq (,$(filter nomac,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/sys/net/include/
endif

include $(RIOTBASE)/Makefile.base
