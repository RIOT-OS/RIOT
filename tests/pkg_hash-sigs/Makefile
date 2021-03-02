include ../Makefile.tests_common

USEMODULE += embunit
USEMODULE += random
USEPKG += hash-sigs

CFLAGS += -DTHREAD_STACKSIZE_MAIN="(THREAD_STACKSIZE_DEFAULT + (20*1024))"

include $(RIOTBASE)/Makefile.include
