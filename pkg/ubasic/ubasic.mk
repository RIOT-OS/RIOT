MODULE = ubasic

# some toolchains complain about the usage of isdigit with a pointer in the
# tokenizer as well as setting a non-const pointer with strchr
CFLAGS += -Wno-char-subscripts
CFLAGS += -Wno-discarded-qualifiers

SRC := tokenizer.c ubasic.c
NO_AUTO_SRC := 1

include $(RIOTBASE)/Makefile.base
