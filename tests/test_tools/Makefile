DEVELHELP = 0
include ../Makefile.tests_common

USEMODULE += shell

TEST_ON_CI_WHITELIST += all

# Disable shell echo and prompt to not have them in the way for testing
CFLAGS += -DSHELL_NO_ECHO=1 -DSHELL_NO_PROMPT=1

include $(RIOTBASE)/Makefile.include
