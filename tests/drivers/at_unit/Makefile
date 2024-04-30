include ../Makefile.drivers_common

HANDLE_URC ?= 1
ECHO_ON ?= 1
SEND_EOL ?= "\\xd"
RECV_EOL_1 ?= "\\xd"
RECV_EOL_2 ?= "\\xa"

ifeq ($(HANDLE_URC), 1)
	USEMODULE += at_urc
endif

ifeq ($(ECHO_ON), 0)
	CFLAGS += -DCONFIG_AT_SEND_SKIP_ECHO=1
endif

CFLAGS += -DAT_RECV_EOL_1="\"$(RECV_EOL_1)\""
CFLAGS += -DAT_RECV_EOL_2="\"$(RECV_EOL_2)\""
CFLAGS += -DCONFIG_AT_SEND_EOL="\"$(SEND_EOL)\""

USEMODULE += embunit
USEMODULE += at

include $(RIOTBASE)/Makefile.include
