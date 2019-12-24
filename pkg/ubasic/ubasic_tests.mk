MODULE = ubasic_tests

# tests.c has constants that may overflow on <32 bit platforms
CFLAGS += -Wno-overflow
# ubasic_init_peek_poke used in those tests is not exposed
CFLAGS += -Wno-implicit-function-declaration
# LLVM doesn't like ubasic_init_peek_poke due to strict prototyping either
CFLAGS += -Wno-strict-prototypes

SRC := tests.c
NO_AUTO_SRC := 1

include $(RIOTBASE)/Makefile.base
