include ../Makefile.tests_common

USEMODULE += fmt
USEMODULE += posix_semaphore
USEMODULE += ztimer64_usec

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
