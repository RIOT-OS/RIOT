APPLICATION = openthread_test

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP -Wall

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEMODULE+=openthread

#Comment this line for using RIOT shell with OpenThread
#USEMODULE+=openthread_cli


ifneq (,$(filter samr21-xpro,$(BOARD)))
  DRIVER := at86rf233
endif
ifneq (,$(filter iotlab-m3 fox,$(BOARD)))
  DRIVER := at86rf231
endif

ifneq (,$(filter at86rf2%,$(DRIVER)))
  FEATURES_REQUIRED = periph_spi periph_gpio
endif

USEMODULE += $(DRIVER)

USEMODULE+=random

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += ipv6_addr

#required for C++ compiling
CXXEXFLAGS += -fno-rtti
USEMODULE += cpp11-compat

include $(RIOTBASE)/Makefile.include
