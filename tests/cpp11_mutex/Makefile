# name of your application
APPLICATION = cpp11_mutex

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# ROM is overflowing for these boards when using
# gcc-arm-none-eabi-4.9.3.2015q2-1trusty1 from ppa:terry.guo/gcc-arm-embedded
# (Travis is using this PPA currently, 2015-06-23)
# Debian jessie libstdc++-arm-none-eabi-newlib-4.8.3-9+4 works fine, though.
# Remove this line if Travis is upgraded to a different toolchain which does
# not pull in all C++ locale code whenever exceptions are used.
BOARD_INSUFFICIENT_MEMORY := stm32f0discovery spark-core nucleo-f334

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# If you want to add some extra flags when compile c++ files, add these flags
# to CXXEXFLAGS variable
CXXEXFLAGS += -std=c++11

USEMODULE += cpp11-compat
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
