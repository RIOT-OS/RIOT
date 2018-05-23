include ../Makefile.tests_common

# ROM is overflowing for these boards when using
# gcc-arm-none-eabi-4.9.3.2015q2-1trusty1 from ppa:terry.guo/gcc-arm-embedded
# (Travis is using this PPA currently, 2015-06-23)
# Debian jessie libstdc++-arm-none-eabi-newlib-4.8.3-9+4 works fine, though.
# Remove this line if Travis is upgraded to a different toolchain which does
# not pull in all C++ locale code whenever exceptions are used.
BOARD_INSUFFICIENT_MEMORY := nucleo-f334r8 spark-core stm32f0discovery

# If you want to add some extra flags when compile c++ files, add these flags
# to CXXEXFLAGS variable
CXXEXFLAGS += -std=c++11

USEMODULE += cpp11-compat
USEMODULE += xtimer
USEMODULE += timex

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
