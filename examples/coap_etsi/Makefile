export PROJECT = coap_etsi

# If no BOARD is found in the environment, use this default:
export BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
export RIOTBASE ?= $(CURDIR)/../..

ifeq ($(BOARD),stm32f4discovery)
	include Makefile.$(BOARD)
endif

USEPKG=libcoap

USEMODULE += auto_init
USEMODULE += shell
USEMODULE += vtimer

USEMODULE += posix
USEMODULE += pnet
USEMODULE += pthread

ifeq ($(strip $(BOARD)),native)
	USEMODULE += nativenet
else ifeq ($(strip $(BOARD)),msba2)
	USEMODULE += cc110x_ng
endif

#export INCLUDES += -I$(RIOTBASE)/pkg/libcoap/libcoap


CFLAGS += -isystem $(RIOTBASE)/sys/net/include \
		  -isystem $(RIOTBASE)/sys/posix/pnet/include \
		  -isystem $(RIOTBASE)/sys/posix/pthread/include

export INCLUDES += -I$(RIOTBASE)/sys/net/include \
				   -I$(RIOTBASE)/sys/posix/pnet/include \
				   -I$(RIOTBASE)/sys/posix/include \
				   -I$(RIOTBASE)/sys/posix/pthread/include


include $(RIOTBASE)/Makefile.include
