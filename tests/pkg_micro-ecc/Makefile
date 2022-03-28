include ../Makefile.tests_common

USEMODULE += hashes
USEPKG += micro-ecc

include $(RIOTBASE)/Makefile.include

# Increase Stack size for AVR
ifneq (,$(filter avr8_common,$(USEMODULE)))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_LARGE
endif
