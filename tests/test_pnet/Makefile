export PROJECT =test_pnet
include ../Makefile.tests_common

USEMODULE += auto_init
USEMODULE += posix
USEMODULE += pnet
USEMODULE += vtimer
USEMODULE += defaulttransceiver

ifeq ($(BOARD),native)
	CFLAGS += -isystem $(RIOTBASE)/sys/net/include \
			  -isystem $(RIOTBASE)/sys/posix/pnet/include
else
	export INCLUDES += -I$(RIOTBASE)/sys/net/include \
					   -I$(RIOTBASE)/sys/posix/pnet/include \
					   -I$(RIOTBASE)/sys/posix/include
endif

include $(RIOTBASE)/Makefile.include
