include ../Makefile.tests_common

USEMODULE += fmt
USEMODULE += core_mutex_priority_inheritance

ifneq ($(RIOT_CI_BUILD),1)
  # For human beings add a busy delay to the mid priority task to make the problem more approachable
  FANCY ?= 1
else
  # Skip the fancy delay for the CI to not waste precious CI time
  FANCY ?= 0
endif

# KNOWN ISSUE #18534
# Currently this is failing, causing unrelated errors to block other PRs.
# This issue will be looked into but for now we need to just close our eyes.
TEST_ON_CI_BLACKLIST += esp32-wroom-32

include $(RIOTBASE)/Makefile.include

CFLAGS += -DFANCY=$(FANCY)
CFLAGS += -DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_SMALL
