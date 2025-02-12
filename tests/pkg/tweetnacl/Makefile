include ../Makefile.pkg_common

TEST_ON_CI_WHITELIST += native native32

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(5*THREAD_STACKSIZE_DEFAULT\)

USEPKG += tweetnacl
USEMODULE += embunit

include $(RIOTBASE)/Makefile.include
