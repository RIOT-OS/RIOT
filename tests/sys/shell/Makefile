DEVELHELP=0
include ../Makefile.sys_common

USEMODULE += app_metadata
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += ztimer_msec

# Use a terminal that does not introduce extra characters into the stream.
RIOT_TERMINAL ?= socat

APP_SHELL_FMT ?= NONE

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

# requires #19005
TEST_ON_CI_BLACKLIST += native native64

include $(RIOTBASE)/Makefile.include

CFLAGS += '-DTHREAD_STACKSIZE_MAIN=(THREAD_STACKSIZE_SMALL+THREAD_EXTRA_STACKSIZE_PRINTF)'

# the test script skips tests if socat is not used
$(call target-export-variables,$(RIOT_TERMINAL),RIOT_TERMINAL)

# skip reboot for boards using highlevel_stdio that would disconnect during reboot
ifneq (,$(filter highlevel_stdio,$(FEATURES_USED)))
  TESTRUNNER_SHELL_SKIP_REBOOT := 1
endif
$(call target-export-variables,$(TESTRUNNER_SHELL_SKIP_REBOOT),TESTRUNNER_SHELL_SKIP_REBOOT)
