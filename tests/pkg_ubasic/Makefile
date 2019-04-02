include ../Makefile.tests_common

TEST_ON_CI_WHITELIST += all

PKG_UNDER_TEST = ubasic
PKG_TEST_FILE = tests.c

USEPKG += $(PKG_UNDER_TEST)

# ubasic_init_peek_poke used in those tests is not exposed
CFLAGS += -Wno-implicit-function-declaration

BUILDDEPS += $(CURDIR)/main.c

include $(RIOTBASE)/Makefile.include

$(CURDIR)/main.c: $(PKGDIRBASE)/$(PKG_UNDER_TEST)/$(PKG_TEST_FILE)
	@cp $< $@

$(PKGDIRBASE)/$(PKG_UNDER_TEST)/$(PKG_TEST_FILE): | pkg-prepare

clean: ..rm-main

..rm-main:
	@rm -f main.c
