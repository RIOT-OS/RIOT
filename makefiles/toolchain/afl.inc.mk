include $(RIOTMAKE)/toolchain/llvm.inc.mk

CC     = $(PREFIX)afl-clang-fast
CXX    = $(PREFIX)afl-clang-fast++
LINK   = $(PREFIX)afl-clang-fast
LINKXX = $(PREFIX)afl-clang-fast++
