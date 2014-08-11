
SENDER ?= 0
ifeq (0,$(SENDER))
    export APPLICATION = netdev_receiver
else
    export APPLICATION = netdev_sender
endif

include ../Makefile.tests_common

USEMODULE += defaulttransceiver
USEMODULE += netdev_base

DISABLE_MODULE += auto_init

INCLUDES += -I$(RIOTBASE)/sys/net/include

CHANNEL ?= 1
SENDER_ADDRESS ?= 1
RECEIVER_ADDRESS ?= 2
NID ?= 0

ifeq (native,$(BOARD))
    RECEIVER_PORT ?= tap0
    SENDER_PORT ?= tap1
else
    RECEIVER_PORT ?= /dev/ttyUSB1
    SENDER_PORT ?= /dev/ttyUSB0
endif

ifeq (0,$(SENDER))
    CFLAGS += -DRECEIVER
    CFLAGS += -DNETDEV_TEST_ADDRESS=$(RECEIVER_ADDRESS)
    PORT ?= $(RECEIVER_PORT)
else
    CFLAGS += -DSENDER
    CFLAGS += -DNETDEV_TEST_ADDRESS=$(SENDER_ADDRESS)
    PORT ?= $(SENDER_PORT)
endif

include $(RIOTBASE)/Makefile.include

CFLAGS += -DNETDEV_TEST_CHANNEL=$(CHANNEL)
CFLAGS += -DNETDEV_TEST_NID=$(NID)
CFLAGS += -DNETDEV_TEST_RECEIVER=$(RECEIVER_ADDRESS)
CFLAGS += -DNETDEV_TEST_SENDER=$(SENDER_ADDRESS)

test:
	./tests/01-interaction.py
