APPLICATION = driver_mpu_common
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_i2c

USEMODULE += mpu_common
USEMODULE += xtimer

# define parameters for selected boards
ifneq (,$(filter msbiot,$(BOARD)))
  TEST_I2C       ?= MPU9150_I2C
  TEST_HW_ADDR   ?= MPU9150_HW_ADDR
  TEST_COMP_ADDR ?= MPU9150_COMP_ADDR
endif

# set default device parameters in case they are undefined
TEST_I2C       ?= I2C_0
TEST_HW_ADDR   ?= MPU_HW_ADDR_HEX_68
TEST_COMP_ADDR ?= MPU_COMP_ADDR_HEX_0C

# export parameters
CFLAGS += -DTEST_I2C=$(TEST_I2C)
CFLAGS += -DTEST_HW_ADDR=$(TEST_HW_ADDR)
CFLAGS += -DTEST_COMP_ADDR=$(TEST_COMP_ADDR)
#CFLAGS += -DMPU_WITH_COMPASS
include $(RIOTBASE)/Makefile.include
