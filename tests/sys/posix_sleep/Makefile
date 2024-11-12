include ../Makefile.sys_common

USEMODULE += posix_sleep

# Interactive sync improves accuracy of timestamping the output
# and is also needed on native for the automatic test to
# pass. Hence, we just depend on it here.
USEMODULE += test_utils_interactive_sync

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
