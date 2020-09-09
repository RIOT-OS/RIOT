include ../Makefile.tests_common

USEMODULE += embunit
USEMODULE += random
USEPKG += c25519

include $(RIOTBASE)/Makefile.include

# c25519 takes up to 1.5K in stack, almost independent of the platform
ifneq (,$(filter cortex-%,$(CPU_CORE)))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(THREAD_STACKSIZE_DEFAULT+THREAD_EXTRA_STACKSIZE_PRINTF\)
else
ifneq (,$(filter atmega_common msp430_common,$(USEMODULE)))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(5*THREAD_STACKSIZE_DEFAULT+THREAD_EXTRA_STACKSIZE_PRINTF\)
else
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(3*THREAD_STACKSIZE_DEFAULT+THREAD_EXTRA_STACKSIZE_PRINTF\)
endif
endif
