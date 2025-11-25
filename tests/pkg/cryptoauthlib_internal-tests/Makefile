BOARD ?= nucleo-f767zi

include ../Makefile.pkg_common

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(3*THREAD_STACKSIZE_DEFAULT\)
# Test fails to build for these boards fails due to
# redefinition of define AES_COUNT in library, which
# is also defined in board header files
BOARD_BLACKLIST := stk3200 stk3600 stk3700

# due to memory constrains we ignore the cert test
CFLAGS += -DDO_NOT_TEST_CERT
USEPKG += cryptoauthlib
USEMODULE += cryptoauthlib_test

include $(RIOTBASE)/Makefile.include
