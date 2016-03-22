APPLICATION = driver_bmp180
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += bmp180
USEMODULE += xtimer
USEMODULE += printf_float

# set default device parameters in case they are undefined
TEST_I2C                  ?= I2C_0
TEST_MEASURE_OVERSAMPLING ?= BMP180_ULTRALOWPOWER
TEST_ALTITUDE             ?= 158 # altitude in Polytechnique School campus

# export parameters
CFLAGS += -DTEST_I2C=$(TEST_I2C)
CFLAGS += -DTEST_MEASURE_OVERSAMPLING=$(TEST_MEASURE_OVERSAMPLING)
CFLAGS += -DTEST_ALTITUDE=$(TEST_ALTITUDE)

include $(RIOTBASE)/Makefile.include
