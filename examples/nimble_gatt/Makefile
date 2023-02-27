# name of your application
APPLICATION = nimble_gatt

# If no BOARD is found in the environment, use this default:
BOARD ?= pulga

# default serial port
PORT ?= /dev/ttyUSB0

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Include NimBLE
USEPKG += nimble
USEMODULE += nimble_svc_gap
USEMODULE += nimble_svc_gatt

# Use automated advertising
USEMODULE += nimble_autoadv
CFLAGS += -DNIMBLE_AUTOADV_DEVICE_NAME='"NimBLE GATT Example"'
CFLAGS += -DNIMBLE_AUTOADV_START_MANUALLY=1

SENSORS += accelerometer

#BMI160 MODULE
ifeq (pulga,$(BOARD))
FEATURES_REQUIRED += periph_i2c
#USEMODULE += i2c
endif

# Modules to include:
USEMODULE += shell_commands
#USEMODULE += shell_cmds
USEMODULE += ps
USEMODULE += printf_float
USEMODULE += ztimer
USEMODULE += ztimer_msec

USEMODULE += event_timeout_ztimer

ifneq (,$(filter accelerometer,$(SENSORS)))
CFLAGS += -DPULGA_ENABLE_ACCELEROMETER=1
endif

CFLAGS += -DPULGA_USE_RINGBUFFER=1

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
