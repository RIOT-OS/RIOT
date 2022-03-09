include ../Makefile.tests_common

USEMODULE += sps30
USEMODULE += ztimer
USEMODULE += ztimer_msec

CFLAGS+=-DPARAM_SPS30_I2C=I2C_DEV\(0\)
CFLAGS+=-DI2C0_SPEED=I2C_SPEED_NORMAL

include $(RIOTBASE)/Makefile.include
