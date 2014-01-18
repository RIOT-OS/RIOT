export PROJECT = test_net_if

BOARD_WHITELIST = native msba2 telosb

include ../Makefile.tests_common

ifeq ($(BOARD),stm32f4discovery)
	include Makefile.$(BOARD)
endif

USEMODULE += auto_init
USEMODULE += net_if
ifeq ($(BOARD),native)
	USEMODULE += nativenet
else ifeq ($(BOARD),msba2)
	USEMODULE += cc110x_ng
else ifeq ($(BOARD),telosb)
	USEMODULE += cc2420
else ifeq ($(BOARD),wsn430-v1_4)
	USEMODULE += cc2420
else ifeq ($(BOARD),iot-lab_M3)
	USEMODULE += at86rf231
endif

export INCLUDES += -I$(RIOTBASE)/sys/net/include

ifeq ($(BOARD),msba2)
	export INCLUDES += -I$(RIOTBASE)/drivers/cc110x_ng/include \
					   -I$(RIOTBASE)/boards/msba2-common/include
endif

include $(RIOTBASE)/Makefile.include
