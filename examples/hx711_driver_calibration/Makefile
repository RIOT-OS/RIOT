# name of your application
APPLICATION = hx711_driver_calibration

# Use the ST B-L072Z-LRWAN1 board by default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

FEATURES_REQUIRED += periph_gpio
USEMODULE += xtimer
USEMODULE += hx711

# Change these values, if your device is connected on different pins
CFLAGS += "-DHX711_PARAM_SCK=GPIO_PIN(1, 13)"
CFLAGS += "-DHX711_PARAM_DOUT=GPIO_PIN(1, 14)"

CFLAGS += "-DHX711_PARAM_OFFSET=95600"
CFLAGS += "-DHX711_PARAM_SCALE=333"

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include