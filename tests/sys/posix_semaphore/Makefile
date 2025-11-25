include ../Makefile.sys_common

USEMODULE += fmt
USEMODULE += posix_semaphore
USEMODULE += ztimer64_usec

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

# native / native64 may fail under load with:
#    first: waited too long <NUM> usec => FAILED
TEST_ON_CI_BLACKLIST += native native64

include $(RIOTBASE)/Makefile.include
