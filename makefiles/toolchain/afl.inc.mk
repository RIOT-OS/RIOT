include $(RIOTMAKE)/toolchain/gnu.inc.mk

CC     = $(PREFIX)afl-gcc-fast
CXX    = $(PREFIX)afl-g++-fast
LINK   = $(PREFIX)afl-gcc-fast
LINKXX = $(PREFIX)afl-g++-fast

ifeq (1,$(INSIDE_DOCKER))
  # The Ubuntu version used for riotdocker (Ubuntu 26.04.1 LTS)
  # currently ships a version of afl++ that is built against
  # an older version of gcc. The differences seem minimal,
  # so disable the check for now.
  # see https://bugs.launchpad.net/ubuntu/+source/aflplusplus/+bug/2046155
  export AFL_GCC_DISABLE_VERSION_CHECK = 1
endif
