APPLICATION = driver_srf02
include ../Makefile.tests_common

USEMODULE += xtimer
USEMODULE += srf02
USEMODULE += shell

# if you want non-default values for I2C and address, override them using the
# CFLAGS environment variable like:
# CFLAGS="-DSRF02_PARAM_I2C=I2C_DEV(x) -DSRF02_PARAM_ADDR=123"

include $(RIOTBASE)/Makefile.include
