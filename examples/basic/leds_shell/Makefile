# Set the name of your application:
APPLICATION = leds_shell

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../

# Uncomment this to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP = 1

# Change this to 0 to show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += shell
USEMODULE += periph_gpio

# Prevents native and native64 program from segfault when gpio set/clear are
# used without a gpiochip
ifneq (,$(filter native native32 native64,$(BOARD)))
 USEMODULE += periph_gpio_mock
endif

include $(RIOTBASE)/Makefile.include
