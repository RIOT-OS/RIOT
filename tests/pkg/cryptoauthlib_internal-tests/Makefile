BOARD ?= nucleo-f767zi

include ../Makefile.pkg_common

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(3*THREAD_STACKSIZE_DEFAULT\)
# Test fails to build for these boards fails due to
# redefinition of define AES_COUNT in library, which
# is also defined in board header files
BOARD_BLACKLIST := stk3200 stk3600 stk3700

# Native boards are blacklisted due to a bug in cryptoauthlib's cmd-processor.c
# The file checks for __linux__ before checking for RIOT_APPLICATION, causing it
# to compile a main() function instead of atca_run_cmd(), which conflicts with
# RIOT's main(). This is an upstream bug in cryptoauthlib test infrastructure.
# See: build/pkg/cryptoauthlib/test/cmd-processor.c lines 233-278
BOARD_BLACKLIST += native native32 native64

# due to memory constrains we ignore the cert test
CFLAGS += -DDO_NOT_TEST_CERT
USEPKG += cryptoauthlib
USEMODULE += cryptoauthlib_test

include $(RIOTBASE)/Makefile.include
