DEVELHELP = 0
include ../Makefile.tests_common

# for some reason pyterm sends ANSI characters when used with graphical
# terminal emulators. Socat doesn't seem to do it.
RIOT_TERMINAL ?= socat

USEMODULE += shell

# Disable shell echo and prompt to not have them in the way for testing
CFLAGS += -DSHELL_NO_ECHO=1 -DSHELL_NO_PROMPT=1

# No need for test_utils_interactive_sync in this test since the test
# synchronizes by itself through `shellping` command.
DISABLE_MODULE += test_utils_interactive_sync

# include sys/test_utils/dummy_thread
USEMODULE += dummy_thread

include $(RIOTBASE)/Makefile.include
