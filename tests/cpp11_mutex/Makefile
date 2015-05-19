# name of your application
APPLICATION = cpp11_mutex

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP -Wno-deprecated

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

BOARD_WHITELIST := stm32f4discovery native

# If you want to add some extra flags when compile c++ files, add these flags
# to CXXEXFLAGS variable
CXXEXFLAGS += -std=c++11 -g -O0 -Wno-deprecated

USEMODULE += cpp11-compat
USEMODULE += vtimer
USEMODULE += timex

include $(RIOTBASE)/Makefile.include
