include ../Makefile.drivers_common

# required modules
USEMODULE += ir_nec

# Setup pin for ir-receiver
# Any interrupt capable GPIO pin should work fine
ifneq (,$(filter b-l475e-iot01a,$(BOARD)))
	# This is the pin marked D7 on b-l475e-iot01a
	IR_PIN ?= GPIO_PIN(PORT_A,4)
endif

# fallback: set some default for compile test
IR_PIN ?= GPIO_PIN(0,0)

CFLAGS += -D'IR_NEC_PARAM_PIN=$(IR_PIN)'

include $(RIOTBASE)/Makefile.include
