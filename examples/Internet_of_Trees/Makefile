# name of your application
APPLICATION = nimble_heart_rate_sensor

# If no BOARD is found in the environment, use this default:
BOARD ?= pulga

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

DRIVER ?= bme280_i2c
USEMODULE += $(DRIVER)
FEATURES_REQUIRED += periph_i2c
USEMODULE += checksum
USEMODULE += xtimer

CFLAGS += -DBMX280_PARAM_I2C_DEV=I2C_DEV\(0\)
CFLAGS += -DBMX280_PARAM_I2C_ADDR=0x77


# Some RIOT modules needed for this example
USEMODULE += event_timeout_ztimer

# Include NimBLE
USEPKG += nimble
USEMODULE += nimble_svc_gap
USEMODULE += nimble_svc_gatt

# Use automated advertising
USEMODULE += nimble_autoadv
CFLAGS += -DNIMBLE_AUTOADV_DEVICE_NAME='"Internet of Trees"'
CFLAGS += -DNIMBLE_AUTOADV_START_MANUALLY=1

# printar float
USEMODULE += printf_float

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
