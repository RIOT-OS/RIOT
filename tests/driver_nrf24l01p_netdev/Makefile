
SENDER ?= 0
ifeq (0,$(SENDER))
    export APPLICATION = nrf24l01p_netdev_receiver
else
    export APPLICATION = nrf24l01p_netdev_sender
endif

include ../Makefile.tests_common

USEMODULE += defaulttransceiver
USEMODULE += net_help
USEMODULE += netdev_base

DISABLE_MODULE += auto_init

ifneq (,$(filter stm32f4discovery,$(BOARD)))
CHANNEL ?= 1
SENDER_ADDRESS ?= 1
RECEIVER_ADDRESS ?= 1
NID ?= 0
else
CHANNEL ?= 1
SENDER_ADDRESS ?= 1
RECEIVER_ADDRESS ?= 2
NID ?= 0
endif

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
