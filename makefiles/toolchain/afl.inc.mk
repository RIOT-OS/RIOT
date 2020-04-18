include $(RIOTMAKE)/toolchain/gnu.inc.mk

export CC     = $(PREFIX)afl-gcc
export CXX    = $(PREFIX)afl-g++
export LINK   = $(PREFIX)afl-gcc
export LINKXX = $(PREFIX)afl-g++
