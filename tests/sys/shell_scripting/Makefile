include ../Makefile.sys_common

USEMODULE += shell
USEMODULE += vfs_default
USEMODULE += vfs_util
USEMODULE += ztimer_msec

DISABLE_MODULE += test_utils_interactive_sync

# only boards with MTD are eligible
TEST_ON_CI_WHITELIST += native native32

include $(RIOTBASE)/Makefile.include
