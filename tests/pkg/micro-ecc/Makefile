include ../Makefile.pkg_common

USEMODULE += hashes
USEMODULE += od
USEPKG += micro-ecc

include $(RIOTBASE)/Makefile.include

# Increase Stack size for AVR
ifneq (,$(filter avr8_common,$(USEMODULE)))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_LARGE
endif

# Access internal API for unit tests
CFLAGS += -DuECC_ENABLE_VLI_API=1
