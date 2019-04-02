include ../Makefile.tests_common

TEST_ON_CI_WHITELIST += all

USEPKG += ubasic

# ubasic_init_peek_poke used in those tests is not exposed
CFLAGS += -Wno-implicit-function-declaration

BUILDDEPS += $(CURDIR)/main.c

include $(RIOTBASE)/Makefile.include

$(CURDIR)/main.c: $(PKGDIRBASE)/ubasic/tests.c
	@cp $< $@

(PKGDIRBASE)/ubasic/tests.c: | pkg-prepare

clean: ..rm-main

..rm-main:
	@rm -f main.c
