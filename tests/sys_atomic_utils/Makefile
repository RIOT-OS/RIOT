include ../Makefile.tests_common

USEMODULE += atomic_utils
USEMODULE += fmt
USEMODULE += random
USEMODULE += shell
USEMODULE += ztimer_usec

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
