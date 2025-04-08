include ../Makefile.drivers_common

# uncomment and adjust the driver's params as needed below if the board does not
# already define a 1-wire bus and its params
#CFLAGS += -DSOFT_ONEWIRE_PARAMS_PIN="GPIO_PIN(PORT_C, 0)"
#CFLAGS += -DSOFT_ONEWIRE_PARAMS_TX_PIN="GPIO_PIN(PORT_C, 0)"
#CFLAGS += -DSOFT_ONEWIRE_PARAMS_RX_PIN="GPIO_PIN(PORT_C, 1)"

# uncomment to use a dedicated hardware timer for the 1-wire bus
# Note that TIMER_DEV(0) is often used by ztimer. This test does not need
# ztimer, when soft_onewire_hwtimer is used. So this is not a problem unless
# ztimer gets enabled for some other reason (such as it being depended upon by
# the board)
#USEMODULE += soft_onewire_hwtimer
#CFLAGS += -DSOFT_ONEWIRE_PARAMS_HWTIMER="TIMER_DEV(0)"

USEMODULE += soft_onewire
USEMODULE += tiny_strerror

include $(RIOTBASE)/Makefile.include
