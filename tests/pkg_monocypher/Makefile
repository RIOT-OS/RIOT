include ../Makefile.tests_common

# required for Monocypher (as described in the package documentation)
CFLAGS += "-DTHREAD_STACKSIZE_MAIN=(4096 + THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)"

USEMODULE += embunit
USEMODULE += random
USEPKG += monocypher

include $(RIOTBASE)/Makefile.include
