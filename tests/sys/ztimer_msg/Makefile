include ../Makefile.sys_common

USEMODULE += ztimer_usec

# uncomment this to test using ztimer msec
#USEMODULE += ztimer_msec

# uncomment this to test using ztimer sec
#USEMODULE += ztimer_sec

# The test is sensitive to background CPU load. On the CI workers a lot of
# compilation tasks are run in parallel, making this test randomly fail.
TEST_ON_CI_BLACKLIST += native native64

include $(RIOTBASE)/Makefile.include
