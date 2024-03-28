DEVELHELP=0
include ../Makefile.sys_common

USEMODULE += shell
USEMODULE += shell_cmds_default

USEMODULE += shell_lock
USEMODULE += shell_lock_auto_locking

ifneq (1, $(SHOULD_RUN_KCONFIG))
  CFLAGS += -DCONFIG_SHELL_LOCK_PASSWORD=\"password\"
  CFLAGS += -DCONFIG_SHELL_LOCK_AUTO_LOCK_TIMEOUT_MS=7000
endif

# test_utils_interactive_sync_shell assumes that the prompt is always '> ' which breaks
# with the password prompt of the shell_lock module which is different from the shell's prompt
DISABLE_MODULE += test_utils_interactive_sync_shell

# for z1, socat doesn't work (unknown reason)
ifeq (z1, $(BOARD))
  RIOT_TERMINAL ?= pyterm
endif

# Use a terminal that does not introduce extra characters into the stream.
RIOT_TERMINAL ?= socat

include $(RIOTBASE)/Makefile.include

# the test script skips tests if socat is not used
$(call target-export-variables,$(RIOT_TERMINAL),RIOT_TERMINAL)
