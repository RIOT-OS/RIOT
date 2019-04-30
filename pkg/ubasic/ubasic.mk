MODULE = ubasic

# some toolchains complain about the usage of isdigit with a pointer in the
# tokenizer
CFLAGS += -Wno-char-subscripts

SRC := tokenizer.c ubasic.c
NO_AUTO_SRC := 1

include $(RIOTBASE)/Makefile.base
