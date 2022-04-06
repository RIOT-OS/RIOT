include ../Makefile.tests_common

USEPKG += umorse

USEMODULE += ztimer

UMORSE_DELAY_DIT ?= 120
CFLAGS += -DUMORSE_DELAY_DIT=$(UMORSE_DELAY_DIT)

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
