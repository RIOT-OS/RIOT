include ../Makefile.tests_common

ifeq (,$(filter mcp47xx,$(USEMODULE)))
    # default expander module
    USEMODULE += mcp47xx
endif

USEMODULE += shell
USEMODULE += benchmark

include $(RIOTBASE)/Makefile.include
