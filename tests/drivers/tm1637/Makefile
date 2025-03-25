include ../Makefile.drivers_common

USEMODULE += tm1637

USEMODULE += ztimer_sec
USEMODULE += embunit

CFLAGS += "-DTM1637_PARAM_CLK=GPIO_PIN(0, 0)"
CFLAGS += "-DTM1637_PARAM_DIO=GPIO_PIN(0, 1)"

include $(RIOTBASE)/Makefile.include
