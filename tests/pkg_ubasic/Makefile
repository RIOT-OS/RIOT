include ../Makefile.tests_common

TEST_ON_CI_WHITELIST += all

USEPKG += ubasic

# ubasic_init_peek_poke used in those tests is not exposed
CFLAGS += -Wno-implicit-function-declaration

include $(RIOTBASE)/Makefile.include

clean: ..rm-main

..rm-main:
	@rm main.c
