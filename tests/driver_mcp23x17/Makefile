include ../Makefile.tests_common

ifeq (,$(filter mcp23%,$(USEMODULE)))
    # default expander module
    USEMODULE += mcp23017
endif

USEMODULE += benchmark
USEMODULE += shell

INCLUDES += -I$(APPDIR)/include

include $(RIOTBASE)/Makefile.include
