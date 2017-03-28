APPLICATION = driver_lis3mdl
BOARD ?= limifrog-v1
include ../Makefile.tests_common

# only this board is known (yet) to provide the sensor LIS3MDL
BOARD_WHITELIST = limifrog-v1

FEATURES_REQUIRED = periph_i2c periph_gpio

USEMODULE += lis3mdl
USEMODULE += xtimer

ifneq (,$(TEST_LIS3MDL_I2C))
	CFLAGS += -DTEST_LIS3MDL_I2C=$(TEST_LIS3MDL_I2C)
else
	# set random default
	CFLAGS += -DTEST_LIS3MDL_I2C=I2C_DEV\(1\)
endif
ifneq (,$(TEST_LIS3MDL_MAG_ADDR))
	CFLAGS += -DTEST_LIS3MDL_MAG_ADDR=$(TEST_LIS3MDL_MAG_ADDR)
else
	# set random default 7 bit address
	CFLAGS += -DTEST_LIS3MDL_MAG_ADDR=28
endif

include $(RIOTBASE)/Makefile.include
