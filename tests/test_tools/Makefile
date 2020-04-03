DEVELHELP = 0
include ../Makefile.tests_common

USEMODULE += shell

# Disable shell echo and prompt to not have them in the way for testing
CFLAGS += -DSHELL_NO_ECHO=1 -DSHELL_NO_PROMPT=1

# No need for test_utils_interactive_sync in this test since the test
# synchronizes by itself through `shellping` command.
DISABLE_MODULE += test_utils_interactive_sync

include $(RIOTBASE)/Makefile.include
