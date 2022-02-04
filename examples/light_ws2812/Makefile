# name of the application
APPLICATION = light_ws2812

# If no BOARD is found in the environment, use this default:
BOARD ?= nucleo-g070rb

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# required packages
USEPKG += light_ws2812
USEMODULE += xtimer
USEMODULE += random

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

CFLAGS += -DLIGHT_WS2812_UC_STM32G0XX

CFLAGS += -DLIGHT_WS2812_GPIO_PIN=7
CFLAGS += -DLIGHT_WS2812_GPIO_PORT=GPIOA

include $(RIOTBASE)/Makefile.include
